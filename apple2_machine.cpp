#include "apple2_machine.h"

#include <cstring>
#include <fstream>
#include <iostream>

using namespace std;

static const uint16_t kTextPage1 = 0x0400;
static const uint16_t kTextPage2 = 0x0800;

static Apple2Machine *active_machine = NULL;

static int readCallback(int addr, int size, uint8_t *dst) {
  return active_machine->memoryRead(addr, size, dst);
}

static int writeCallback(int addr, int size, const uint8_t *src) {
  return active_machine->memoryWrite(addr, size, src);
}

Apple2Machine::Apple2Machine() {
  active_machine = this;
  cpu.setMemoryReadCallBack(readCallback);
  cpu.setMemoryWriteCallBack(writeCallback);
  reset();
}

void Apple2Machine::reset() {
  memset(mem, 0, sizeof(mem));
  key_queue = queue<uint8_t>();
  key_latch = 0;
  key_strobe = false;
  screen_dirty = true;
  text_mode = true;
  mixed_mode = false;
  page2_mode = false;
  hires_mode = false;
}

void Apple2Machine::resetCpu() {
  active_machine = this;
  cpu.reset();
}

void Apple2Machine::step() {
  active_machine = this;
  cpu.step();
}

bool Apple2Machine::loadRom(const string &path, uint16_t addr) {
  ifstream rom(path.c_str(), ios::in | ios::binary);
  if(!rom) {
    cerr << "Failed to open ROM file: " << path << endl;
    return false;
  }

  rom.read((char *)(mem + addr), 65536 - addr);
  return true;
}

void Apple2Machine::installDemoRom() {
  const uint8_t program[] = {
    0xa2, 0x00,             // LDX #$00
    0xbd, 0x1b, 0xf8,       // LDA message,X
    0xf0, 0x06,             // BEQ input_loop
    0x9d, 0x00, 0x04,       // STA $0400,X
    0xe8,                   // INX
    0xd0, 0xf5,             // BNE print_loop
    0xad, 0x00, 0xc0,       // LDA $C000
    0x10, 0xfb,             // BPL input_loop
    0x8d, 0x80, 0x04,       // STA $0480
    0xad, 0x10, 0xc0,       // LDA $C010
    0x4c, 0x0d, 0xf8,       // JMP input_loop
    'A', 'P', 'P', 'L', 'E', ' ', 'I', 'I', ' ', 'C', 'L', 'I', ' ',
    'D', 'E', 'M', 'O', ' ', '-', ' ', 'T', 'Y', 'P', 'E', ' ', 'A',
    ' ', 'K', 'E', 'Y', 0x00
  };

  memcpy(mem + 0xf800, program, sizeof(program));
  mem[0xfffc] = 0x00;
  mem[0xfffd] = 0xf8;
}

int Apple2Machine::memoryRead(int addr, int size, uint8_t *dst) {
  if(dst == NULL) {
    return 0;
  }

  for(int i = 0; i < size; i++) {
    int a = (addr + i) & 0xffff;
    if(a >= 0xc050 && a <= 0xc057) {
      readSoftSwitch(a);
      dst[i] = 0x00;
    } else if(a == 0xc000) {
      if(!key_strobe && !key_queue.empty()) {
        key_latch = key_queue.front();
        key_queue.pop();
        key_strobe = true;
      }
      dst[i] = key_strobe ? (key_latch | 0x80) : 0x00;
    } else if(a == 0xc010) {
      key_strobe = false;
      dst[i] = 0x00;
    } else {
      dst[i] = mem[a];
    }
  }
  return 0;
}

int Apple2Machine::memoryWrite(int addr, int size, const uint8_t *src) {
  if(src == NULL) {
    return 0;
  }

  for(int i = 0; i < size; i++) {
    int a = (addr + i) & 0xffff;
    if(a >= 0xc050 && a <= 0xc057) {
      writeSoftSwitch(a);
    } else if(a == 0xc010) {
      key_strobe = false;
    } else {
      mem[a] = src[i];
      if(a >= kTextPage1 && a < 0x0c00) {
        screen_dirty = true;
      }
    }
  }
  return 0;
}

void Apple2Machine::pushKey(uint8_t c) {
  key_queue.push(c & 0x7f);
}

void Apple2Machine::handleHostKey(uint8_t c) {
  if(c >= 'a' && c <= 'z') c -= 32;
  if(c == 'G') {
    writeSoftSwitch(0xc050);
    writeSoftSwitch(0xc052);
    writeSoftSwitch(0xc056);
    fillLoresDemoPattern();
  } else if(c == 'T') {
    writeSoftSwitch(0xc051);
    writeDemoTitle();
  }
  if(c == 10) c = 13;
  pushKey(c);
}

bool Apple2Machine::isScreenDirty() const {
  return screen_dirty;
}

void Apple2Machine::clearScreenDirty() {
  screen_dirty = false;
}

bool Apple2Machine::isTextMode() const {
  return text_mode;
}

bool Apple2Machine::isMixedMode() const {
  return mixed_mode;
}

bool Apple2Machine::isPage2Mode() const {
  return page2_mode;
}

bool Apple2Machine::isHiresMode() const {
  return hires_mode;
}

uint16_t Apple2Machine::screenAddressForCell(int row, int col) const {
  return textBase() + (row & 0x07) * 0x80 + (row >> 3) * 0x28 + col;
}

char Apple2Machine::textCharAt(int row, int col) const {
  return appleTextChar(mem[screenAddressForCell(row, col)]);
}

char Apple2Machine::loresCharAt(int row, int col) const {
  return appleLoresChar(mem[screenAddressForCell(row, col)], (row & 0x01) != 0);
}

uint8_t Apple2Machine::screenByteAt(int row, int col) const {
  return mem[screenAddressForCell(row, col)];
}

uint16_t Apple2Machine::textBase() const {
  return page2_mode ? kTextPage2 : kTextPage1;
}

char Apple2Machine::appleTextChar(uint8_t value) const {
  char ch = value & 0x7f;
  if(ch < 0x20 || ch == 0x7f) {
    return ' ';
  }
  return ch;
}

char Apple2Machine::appleLoresChar(uint8_t value, bool upperNibble) const {
  static const char palette[] = " .:-=+*#%@ABCDEF";
  uint8_t color = upperNibble ? ((value >> 4) & 0x0f) : (value & 0x0f);
  return palette[color];
}

void Apple2Machine::readSoftSwitch(int addr) {
  writeSoftSwitch(addr);
}

void Apple2Machine::writeSoftSwitch(int addr) {
  switch(addr & 0xffff) {
    case 0xc050: text_mode = false; break;
    case 0xc051: text_mode = true; break;
    case 0xc052: mixed_mode = false; break;
    case 0xc053: mixed_mode = true; break;
    case 0xc054: page2_mode = false; break;
    case 0xc055: page2_mode = true; break;
    case 0xc056: hires_mode = false; break;
    case 0xc057: hires_mode = true; break;
  }
  screen_dirty = true;
}

void Apple2Machine::writeDemoTitle() {
  const char *title = "APPLE II CLI DEMO - TYPE A KEY";
  size_t title_len = strlen(title);
  for(int row = 0; row < 24; row++) {
    for(int col = 0; col < 40; col++) {
      mem[screenAddressForCell(row, col)] = ' ';
    }
  }
  for(int col = 0; col < 40; col++) {
    mem[screenAddressForCell(0, col)] = col < (int)title_len ? (uint8_t)title[col] : ' ';
  }
  screen_dirty = true;
}

void Apple2Machine::fillLoresDemoPattern() {
  for(int row = 0; row < 24; row++) {
    for(int col = 0; col < 40; col++) {
      uint8_t upper = (row + col) & 0x0f;
      uint8_t lower = (row * 3 + col * 2) & 0x0f;
      mem[screenAddressForCell(row, col)] = (upper << 4) | lower;
    }
  }
  screen_dirty = true;
}
