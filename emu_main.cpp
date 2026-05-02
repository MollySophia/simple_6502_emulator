#include <iostream>
#include "emu_core.h"
#include "console.h"
#include <fstream>
#ifdef _WIN32
#include <conio.h>
#include <windows.h>
#else
#include <termios.h>
#include <sys/select.h>
#include <unistd.h>
#endif
#include <cstdlib>
#include <cstring>
#include <queue>
using namespace std;

MOS6502 emu_core;
Console con;
uint8_t mem[65536];
#ifdef _WIN32
static DWORD old_output_mode = 0;
static bool terminal_raw = false;
#else
static struct termios old_tm;
static bool terminal_raw = false;
#endif
static bool emulator_running = true;
static queue<uint8_t> key_queue;

int loadRom(char *filename, uint16_t addr, uint8_t *memory);
int memoryReadCb(int addr, int size, uint8_t *dst);
int memoryWriteCb(int addr, int size, const uint8_t *src);
void setupTerminal(void);
void restoreTerminal(void);
void pollKeyboard(void);
void writeDisplay(uint8_t value);
uint8_t normalizeKey(uint8_t c);

int main() {
  memset(mem, 0, sizeof(mem));
  loadRom((char*)"ROM/Woz Monitor.rom", 0xFF00, mem);
  loadRom((char*)"ROM/Integrated BASIC.hex", 0xE000, mem);
  loadRom((char*)"ROM/A1A90.BIN", 0x9000, mem);
  emu_core.setMemoryReadCallBack(memoryReadCb);
  emu_core.setMemoryWriteCallBack(memoryWriteCb);
  emu_core.reset();

  setupTerminal();
  con.clear_screen();
  fputs("\033[32m", stdout);
  while(emulator_running) {
    pollKeyboard();
    emu_core.step();
  }
  restoreTerminal();
  return 0;
}

int loadRom(char *filename, uint16_t addr, uint8_t *memory) {
  int len = 0;
  ifstream romfile;
  romfile.open(filename, ios::in|ios::binary);
  if(!romfile) {
    cout << "Failed to open ROM files." << endl;
    return -1;
  }
  romfile.seekg(0, ios::end);
  len = romfile.tellg();
  if((addr + len) > 65536) {
    cout << "Load ROM files failed: File too large." << endl;
    return -1;
  }
  romfile.seekg(0);
  romfile.read((char*)(memory + addr), len);
  romfile.close();
  return 0;
}

int memoryReadCb(int addr, int size, uint8_t *dst) {
  if(dst == NULL) {
    return 0;
  }

  for(int i = 0; i < size; i++) {
    int a = (addr + i) & 0xffff;
    if(a == 0xD011) {
      pollKeyboard();
      dst[i] = key_queue.empty() ? 0x00 : 0x80;
    } else if(a == 0xD010) {
      pollKeyboard();
      if(key_queue.empty()) {
        dst[i] = 0x00;
      } else {
        dst[i] = key_queue.front();
        key_queue.pop();
      }
    } else if(a == 0xD012) {
      dst[i] = 0x00;
    } else {
      dst[i] = mem[a];
    }
  }
  return 0;
}

int memoryWriteCb(int addr, int size, const uint8_t *src) {
  if(src == NULL) {
    return 0;
  }

  for(int i = 0; i < size; i++) {
    int a = (addr + i) & 0xffff;
    mem[a] = src[i];
    if(a == 0xD012) {
      writeDisplay(src[i]);
      mem[a] = 0x00;
    }
  }

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
  while(emulator_running) {
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
      emulator_running = false;
      return;
    }

    key_queue.push(normalizeKey(c));
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
      emulator_running = false;
      return;
    }

    key_queue.push(normalizeKey(c));
#endif
  }
}

void writeDisplay(uint8_t value) {
  uint8_t ch = value & 0x7F;
  if(ch == 0x7F) {
    return;
  }
  if(ch == 13) ch = 10;
  if(ch == (0xDF & 0x7F)) {
    fputs("\b \b", stdout);
    fflush(stdout);
    return;
  }
  putc((char)ch, stdout);
  fflush(stdout);
}

uint8_t normalizeKey(uint8_t c) {
  if(c == 10) c = 13;
  if(c == 0x7F) c = 0xDF;
  if(c >= 97 && c <= 122) c -= 32;
  return c | 0x80;
}
