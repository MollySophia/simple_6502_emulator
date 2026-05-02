#include "apple2_machine.h"

#include <cstdlib>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#endif

using namespace std;

static Apple2Machine machine;
static bool running = true;
static const unsigned long kRenderIntervalSteps = 20000;
#ifdef _WIN32
static DWORD old_output_mode = 0;
static bool terminal_raw = false;
#else
static bool terminal_raw = false;
static struct termios old_tm;
#endif

void setupTerminal(void);
void restoreTerminal(void);
void pollKeyboard(void);
void renderScreen(void);
void sleepForFrame(void);
bool runSmokeTest(void);

int main(int argc, char **argv) {
  machine.reset();
  running = true;

  if(argc == 2 && string(argv[1]) == "--smoke-test") {
    return runSmokeTest() ? 0 : 1;
  }

  bool romLoaded = false;
  for(int i = 1; i < argc; i++) {
    string arg = argv[i];
    if(arg == "--rom" && i + 1 < argc) {
      if(!machine.loadSystemRom(argv[++i])) {
        return 1;
      }
      romLoaded = true;
    } else if(arg == "--disk1" && i + 1 < argc) {
      if(!machine.loadDiskImage(argv[++i], 1)) {
        return 1;
      }
    } else if(arg == "--disk2" && i + 1 < argc) {
      if(!machine.loadDiskImage(argv[++i], 2)) {
        return 1;
      }
    } else if(arg == "--slot6-rom" && i + 1 < argc) {
      if(!machine.loadSlotRom(argv[++i], 6)) {
        return 1;
      }
    } else if(arg == "--disk-rom-scaffold") {
      machine.installDiskIIRomScaffold();
    } else {
      cerr << "usage: " << argv[0]
           << " [--rom apple2.rom] [--disk1 disk.dsk] [--disk2 disk.dsk]"
           << " [--slot6-rom rom.bin] [--disk-rom-scaffold]"
           << endl;
      return 1;
    }
  }

  if(!romLoaded) {
    machine.installDemoRom();
  }
  machine.resetCpu();

  setupTerminal();
  fputs("\033[2J\033[?25l", stdout);
  fflush(stdout);
  renderScreen();

  unsigned long steps = 0;
  unsigned long next_render_step = kRenderIntervalSteps;
  while(running) {
    pollKeyboard();
    machine.step();
    steps++;

    if(machine.isScreenDirty() && steps >= next_render_step) {
      renderScreen();
      next_render_step = steps + kRenderIntervalSteps;
      sleepForFrame();
    }
  }

  renderScreen();
  fputs("\033[?25h\033[0m\n", stdout);
  restoreTerminal();
  return 0;
}

void setupTerminal(void) {
#ifdef _WIN32
  HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
  if(output == INVALID_HANDLE_VALUE) {
    return;
  }

  DWORD mode = 0;
  if(!GetConsoleMode(output, &mode)) {
    return;
  }

  old_output_mode = mode;
  mode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
  if(SetConsoleMode(output, mode)) {
    terminal_raw = true;
    atexit(restoreTerminal);
  }
#else
  struct termios tm;
  if(tcgetattr(STDIN_FILENO, &old_tm) < 0) {
    return;
  }

  tm = old_tm;
  cfmakeraw(&tm);
  if(tcsetattr(STDIN_FILENO, TCSANOW, &tm) == 0) {
    terminal_raw = true;
    atexit(restoreTerminal);
  }
#endif
}

void restoreTerminal(void) {
#ifdef _WIN32
  if(terminal_raw) {
    HANDLE output = GetStdHandle(STD_OUTPUT_HANDLE);
    if(output != INVALID_HANDLE_VALUE) {
      SetConsoleMode(output, old_output_mode);
    }
    terminal_raw = false;
  }
#else
  if(terminal_raw) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tm);
    terminal_raw = false;
  }
#endif
}

void pollKeyboard(void) {
  while(running) {
#ifdef _WIN32
    if(!_kbhit()) {
      return;
    }

    int ch = _getch();
    if(ch == 0 || ch == 0xe0) {
      if(_kbhit()) {
        (void)_getch();
      }
      continue;
    }

    uint8_t c = (uint8_t)ch;
    if(c == 3) {
      running = false;
      return;
    }

    machine.handleHostKey(c);
#else
    fd_set readfds;
    struct timeval timeout;
    FD_ZERO(&readfds);
    FD_SET(STDIN_FILENO, &readfds);
    timeout.tv_sec = 0;
    timeout.tv_usec = 0;

    if(select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout) <= 0) {
      return;
    }

    uint8_t c = 0;
    if(read(STDIN_FILENO, &c, 1) != 1) {
      return;
    }

    if(c == 3) {
      running = false;
      return;
    }

    machine.handleHostKey(c);
#endif
  }
}

void sleepForFrame(void) {
#ifdef _WIN32
  Sleep(16);
#else
  usleep(16000);
#endif
}

void renderScreen(void) {
  fputs("\033[H\033[32m", stdout);
  for(int row = 0; row < machine.displayRows(); row++) {
    for(int col = 0; col < 40; col++) {
      putc(machine.displayCharAt(row, col), stdout);
    }
    putc('\n', stdout);
  }
  printf("\033[0mAPPLE II CLI %s %s PAGE %d %s | G graphics | T text | Ctrl-C exits",
         machine.isTextMode() ? "TEXT" : "LORES",
         machine.isMixedMode() ? "MIXED" : "FULL",
         machine.isPage2Mode() ? 2 : 1,
         machine.isHiresMode() ? "HIRES-SW" : "LORES-SW");
  fflush(stdout);
  machine.clearScreenDirty();
}

bool runSmokeTest(void) {
  machine.reset();
  machine.installDemoRom();
  machine.pushKey('Z');
  machine.resetCpu();

  for(unsigned long step = 0; step < 2000000; step++) {
    machine.step();
    if(machine.textCharAt(1, 0) == 'Z') {
      break;
    }
  }

  const string expected = "APPLE II CLI DEMO - TYPE A KEY";
  for(size_t i = 0; i < expected.size(); i++) {
    if(machine.screenByteAt(0, (int)i) != (uint8_t)expected[i]) {
      cerr << "Apple II CLI smoke test failed while checking title" << endl;
      return false;
    }
  }

  if(machine.textCharAt(1, 0) != 'Z') {
    cerr << "Apple II CLI smoke test failed while checking keyboard" << endl;
    return false;
  }

  uint8_t dummy = 0;
  machine.memoryRead(0xc050, 1, &dummy);
  if(machine.isTextMode() || machine.displayRows() != 48) {
    cerr << "Apple II CLI smoke test failed while switching to graphics" << endl;
    return false;
  }

  machine.memoryWrite(machine.screenAddressForCell(0, 0), 1, (const uint8_t *)"\xde");
  if(machine.loresCharAt(0, 0) != 'E' || machine.loresCharAt(1, 0) != 'D') {
    cerr << "Apple II CLI smoke test failed while checking lores rendering" << endl;
    return false;
  }

  machine.memoryRead(0xc055, 1, &dummy);
  machine.memoryWrite(machine.screenAddressForCell(0, 0), 1, (const uint8_t *)"2");
  if(machine.screenAddressForCell(0, 0) != 0x0800 ||
     machine.screenByteAt(0, 0) != '2') {
    cerr << "Apple II CLI smoke test failed while checking page 2" << endl;
    return false;
  }

  machine.memoryRead(0xc054, 1, &dummy);
  if(machine.screenAddressForCell(0, 0) != 0x0400 ||
     machine.screenByteAt(0, 0) == '2') {
    cerr << "Apple II CLI smoke test failed while returning to page 1" << endl;
    return false;
  }

  machine.memoryRead(0xc050, 1, &dummy);
  machine.memoryRead(0xc053, 1, &dummy);
  if(machine.displayRows() != 44 || machine.isDisplayTextRow(39) ||
     !machine.isDisplayTextRow(40)) {
    cerr << "Apple II CLI smoke test failed while checking mixed mode" << endl;
    return false;
  }

  machine.memoryRead(0xc051, 1, &dummy);
  if(!machine.isTextMode()) {
    cerr << "Apple II CLI smoke test failed while switching to text" << endl;
    return false;
  }

  machine.handleHostKey('G');
  if(machine.isTextMode() || machine.screenByteAt(3, 4) == 0x00) {
    cerr << "Apple II CLI smoke test failed while using graphics hotkey" << endl;
    return false;
  }

  machine.handleHostKey('T');
  if(!machine.isTextMode()) {
    cerr << "Apple II CLI smoke test failed while using text hotkey" << endl;
    return false;
  }

  machine.memoryRead(0xc0b0, 1, &dummy);
  if(machine.slotAccessCount(3) != 1) {
    cerr << "Apple II CLI smoke test failed while checking empty slot I/O" << endl;
    return false;
  }

  machine.memoryRead(0xc0e1, 1, &dummy);
  machine.memoryRead(0xc0e5, 1, &dummy);
  machine.memoryRead(0xc0e9, 1, &dummy);
  machine.memoryRead(0xc0eb, 1, &dummy);
  machine.memoryRead(0xc0ed, 1, &dummy);
  machine.memoryRead(0xc0ef, 1, &dummy);
  if(machine.slotAccessCount(6) != 6 || !machine.diskPhaseOn(0) ||
     !machine.diskPhaseOn(2) || !machine.diskMotorOn() ||
     machine.diskDrive() != 2 || !machine.diskQ6() || !machine.diskQ7() ||
     dummy != machine.diskDataLatch()) {
    cerr << "Apple II CLI smoke test failed while checking Disk II slot I/O" << endl;
    return false;
  }

  machine.memoryRead(0xc0e0, 1, &dummy);
  machine.memoryRead(0xc0e8, 1, &dummy);
  machine.memoryRead(0xc0ec, 1, &dummy);
  machine.memoryRead(0xc0ee, 1, &dummy);
  if(machine.diskPhaseOn(0) || machine.diskMotorOn() ||
     machine.diskQ6() || machine.diskQ7()) {
    cerr << "Apple II CLI smoke test failed while clearing Disk II switches" << endl;
    return false;
  }

  vector<uint8_t> disk(35 * 16 * 256, 0x00);
  disk[0] = 0x11;
  disk[1] = 0x22;
  disk[(1 * 16 * 256)] = 0x33;
  disk[(34 * 16 * 256) + 4095] = 0x44;

  const string diskPath = "build/apple2_smoke.dsk";
  ofstream diskFile(diskPath.c_str(), ios::out | ios::binary);
  diskFile.write((const char *)&disk[0], disk.size());
  diskFile.close();

  machine.reset();
  if(!machine.loadDiskImage(diskPath, 1) || !machine.diskInserted(1)) {
    cerr << "Apple II CLI smoke test failed while loading disk image" << endl;
    return false;
  }
  machine.memoryRead(0xc0e9, 1, &dummy);
  machine.memoryRead(0xc0ec, 1, &dummy);
  if(dummy != 0x11) {
    cerr << "Apple II CLI smoke test failed while reading first disk byte" << endl;
    return false;
  }
  machine.memoryRead(0xc0ec, 1, &dummy);
  if(dummy != 0x22) {
    cerr << "Apple II CLI smoke test failed while advancing disk stream" << endl;
    return false;
  }
  machine.memoryRead(0xc0e1, 1, &dummy);
  machine.memoryRead(0xc0e3, 1, &dummy);
  machine.memoryRead(0xc0e5, 1, &dummy);
  if(machine.diskTrack() != 1) {
    cerr << "Apple II CLI smoke test failed while stepping disk track" << endl;
    return false;
  }
  machine.memoryRead(0xc0ec, 1, &dummy);
  if(dummy != 0x33) {
    cerr << "Apple II CLI smoke test failed while reading stepped disk track" << endl;
    return false;
  }

  machine.reset();
  machine.loadDiskImage(diskPath, 1);
  machine.installDiskIIRomScaffold();
  if(!machine.slotRomLoaded(6)) {
    cerr << "Apple II CLI smoke test failed while installing slot ROM" << endl;
    return false;
  }
  machine.memoryRead(0xc600, 1, &dummy);
  if(dummy != 0xad) {
    cerr << "Apple II CLI smoke test failed while reading slot ROM" << endl;
    return false;
  }
  uint8_t overwrite = 0x00;
  machine.memoryWrite(0xc600, 1, &overwrite);
  machine.memoryRead(0xc600, 1, &dummy);
  if(dummy != 0xad) {
    cerr << "Apple II CLI smoke test failed while checking slot ROM write protect" << endl;
    return false;
  }

  vector<uint8_t> rom(0x1000, 0xea);
  rom[0x0000] = 0x4c; // JMP $c600
  rom[0x0001] = 0x00;
  rom[0x0002] = 0xc6;
  rom[0x0ffc] = 0x00;
  rom[0x0ffd] = 0xf0;

  const string romPath = "build/apple2_smoke.rom";
  ofstream romFile(romPath.c_str(), ios::out | ios::binary);
  romFile.write((const char *)&rom[0], rom.size());
  romFile.close();

  machine.reset();
  machine.loadDiskImage(diskPath, 1);
  machine.installDiskIIRomScaffold();
  if(!machine.loadSystemRom(romPath)) {
    cerr << "Apple II CLI smoke test failed while loading system ROM" << endl;
    return false;
  }
  machine.resetCpu();
  for(int i = 0; i < 200; i++) {
    machine.step();
  }
  const string slotMessage = "SLOT 6 DISK ROM";
  for(size_t i = 0; i < slotMessage.size(); i++) {
    if(machine.screenByteAt(0, (int)i) != (uint8_t)slotMessage[i]) {
      cerr << "Apple II CLI smoke test failed while executing slot ROM" << endl;
      return false;
    }
  }
  if(machine.screenByteAt(1, 0) != 0x11) {
    cerr << "Apple II CLI smoke test failed while slot ROM read disk data" << endl;
    return false;
  }

  uint8_t original = 0;
  machine.memoryRead(0xf000, 1, &original);
  machine.memoryWrite(0xf000, 1, &overwrite);
  machine.memoryRead(0xf000, 1, &dummy);
  if(dummy != original) {
    cerr << "Apple II CLI smoke test failed while checking ROM write protect" << endl;
    return false;
  }

  cout << "Apple II CLI smoke test passed" << endl;
  return true;
}
