#include "apple2_machine.h"

#include <cstdlib>
#include <iostream>
#include <string>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>

using namespace std;

static Apple2Machine machine;
static bool running = true;
static bool terminal_raw = false;
static struct termios old_tm;

void setupTerminal(void);
void restoreTerminal(void);
void pollKeyboard(void);
void renderScreen(void);
bool runSmokeTest(void);

int main(int argc, char **argv) {
  machine.reset();
  running = true;

  if(argc == 2 && string(argv[1]) == "--smoke-test") {
    return runSmokeTest() ? 0 : 1;
  }

  if(argc == 3 && string(argv[1]) == "--rom") {
    machine.loadRom(argv[2], 0xd000);
  } else {
    machine.installDemoRom();
  }
  machine.resetCpu();

  setupTerminal();
  fputs("\033[2J\033[?25l", stdout);
  fflush(stdout);

  unsigned long steps = 0;
  while(running) {
    pollKeyboard();
    machine.step();
    steps++;

    if(machine.isScreenDirty() && (steps % 20000) == 0) {
      renderScreen();
      usleep(16000);
    }
  }

  renderScreen();
  fputs("\033[?25h\033[0m\n", stdout);
  restoreTerminal();
  return 0;
}

void setupTerminal(void) {
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
}

void restoreTerminal(void) {
  if(terminal_raw) {
    tcsetattr(STDIN_FILENO, TCSANOW, &old_tm);
    terminal_raw = false;
  }
}

void pollKeyboard(void) {
  while(running) {
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
  }
}

void renderScreen(void) {
  fputs("\033[H\033[32m", stdout);
  for(int row = 0; row < 24; row++) {
    for(int col = 0; col < 40; col++) {
      if(machine.isTextMode() || (machine.isMixedMode() && row >= 20)) {
        putc(machine.textCharAt(row, col), stdout);
      } else {
        putc(machine.loresCharAt(row, col), stdout);
      }
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
  if(machine.isTextMode()) {
    cerr << "Apple II CLI smoke test failed while switching to graphics" << endl;
    return false;
  }

  machine.memoryWrite(machine.screenAddressForCell(0, 0), 1, (const uint8_t *)"\x7e");
  if(machine.loresCharAt(0, 0) != 'E' || machine.loresCharAt(1, 0) != 'D') {
    cerr << "Apple II CLI smoke test failed while checking lores rendering" << endl;
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

  cout << "Apple II CLI smoke test passed" << endl;
  return true;
}
