#include "apple2_machine.h"

#include <cstring>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;

static const uint16_t kTextPage1 = 0x0400;
static const uint16_t kTextPage2 = 0x0800;
static const uint16_t kSystemRomBase = 0xd000;
static const size_t kDiskImageSize = 35 * 16 * 256;

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
  rom_loaded = false;
  rom_start = 0xffff;
  memset(slot_rom_loaded, 0, sizeof(slot_rom_loaded));
  memset(slot_access_count, 0, sizeof(slot_access_count));
  memset(disk_phase, 0, sizeof(disk_phase));
  disk_motor_on = false;
  disk_drive = 1;
  disk_q6 = false;
  disk_q7 = false;
  disk_data_latch = 0x7e;
  memset(disk_inserted, 0, sizeof(disk_inserted));
  disk_image[0].clear();
  disk_image[1].clear();
  disk_stream_pos[0] = 0;
  disk_stream_pos[1] = 0;
  disk_half_track = 0;
  disk_last_phase = 0;
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

bool Apple2Machine::loadSystemRom(const string &path) {
  ifstream rom(path.c_str(), ios::in | ios::binary | ios::ate);
  if(!rom) {
    cerr << "Failed to open ROM file: " << path << endl;
    return false;
  }

  streamoff size = rom.tellg();
  if(size <= 0 || size > (0x10000 - kSystemRomBase)) {
    cerr << "Apple II system ROM must be between 1 byte and 12 KiB: "
         << path << endl;
    return false;
  }

  vector<uint8_t> data((size_t)size);
  rom.seekg(0, ios::beg);
  if(!rom.read((char *)&data[0], size)) {
    cerr << "Failed to read ROM file: " << path << endl;
    return false;
  }

  rom_start = (uint16_t)(0x10000 - data.size());
  if(rom_start < kSystemRomBase) {
    rom_start = kSystemRomBase;
  }
  memcpy(mem + rom_start, &data[0], data.size());
  rom_loaded = true;
  return true;
}

bool Apple2Machine::loadSlotRom(const string &path, int slot) {
  if(slot < 1 || slot > 7) {
    cerr << "Slot ROM slot must be between 1 and 7" << endl;
    return false;
  }

  ifstream rom(path.c_str(), ios::in | ios::binary | ios::ate);
  if(!rom) {
    cerr << "Failed to open slot ROM file: " << path << endl;
    return false;
  }

  streamoff size = rom.tellg();
  if(size <= 0 || size > 256) {
    cerr << "Slot ROM must be between 1 byte and 256 bytes: " << path << endl;
    return false;
  }

  uint16_t base = (uint16_t)(0xc000 + slot * 0x100);
  memset(mem + base, 0xea, 256);
  rom.seekg(0, ios::beg);
  if(!rom.read((char *)(mem + base), size)) {
    cerr << "Failed to read slot ROM file: " << path << endl;
    return false;
  }

  slot_rom_loaded[slot] = true;
  return true;
}

bool Apple2Machine::loadDiskImage(const string &path, int drive) {
  if(drive < 1 || drive > 2) {
    cerr << "Disk drive must be 1 or 2" << endl;
    return false;
  }

  ifstream disk(path.c_str(), ios::in | ios::binary | ios::ate);
  if(!disk) {
    cerr << "Failed to open disk image: " << path << endl;
    return false;
  }

  streamoff size = disk.tellg();
  if(size != (streamoff)kDiskImageSize) {
    cerr << "Only 140 KiB DOS-order .dsk images are supported: "
         << path << endl;
    return false;
  }

  int index = drive - 1;
  disk_image[index].assign(kDiskImageSize, 0);
  disk.seekg(0, ios::beg);
  if(!disk.read((char *)&disk_image[index][0], kDiskImageSize)) {
    cerr << "Failed to read disk image: " << path << endl;
    disk_image[index].clear();
    return false;
  }

  disk_inserted[index] = true;
  disk_stream_pos[index] = 0;
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
  rom_loaded = true;
  rom_start = 0xf800;
}

void Apple2Machine::installDiskIIRomScaffold() {
  const uint8_t program[] = {
    0xad, 0xe9, 0xc0,       // LDA $C0E9 ; motor on
    0xad, 0xec, 0xc0,       // LDA $C0EC ; read disk latch
    0x8d, 0x80, 0x04,       // STA $0480
    0xa2, 0x00,             // LDX #$00
    0xbd, 0x1c, 0xc6,       // LDA message,X
    0xf0, 0x09,             // BEQ done
    0x9d, 0x00, 0x04,       // STA $0400,X
    0xe8,                   // INX
    0xd0, 0xf5,             // BNE print_loop
    0x4c, 0x19, 0xc6,       // JMP done
    0x4c, 0x19, 0xc6,       // done: JMP done
    'S', 'L', 'O', 'T', ' ', '6', ' ', 'D', 'I', 'S', 'K', ' ',
    'R', 'O', 'M', 0x00
  };

  memset(mem + 0xc600, 0xea, 256);
  memcpy(mem + 0xc600, program, sizeof(program));
  slot_rom_loaded[6] = true;
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
    } else if(a >= 0xc080 && a <= 0xc0ff) {
      dst[i] = accessSlotIo(a);
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
    } else if(a >= 0xc080 && a <= 0xc0ff) {
      accessSlotIo(a);
    } else {
      if(!isProtectedRomAddress(a)) {
        mem[a] = src[i];
      }
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

unsigned int Apple2Machine::slotAccessCount(int slot) const {
  if(slot < 0 || slot >= 8) {
    return 0;
  }
  return slot_access_count[slot];
}

bool Apple2Machine::diskMotorOn() const {
  return disk_motor_on;
}

int Apple2Machine::diskDrive() const {
  return disk_drive;
}

bool Apple2Machine::diskPhaseOn(int phase) const {
  if(phase < 0 || phase >= 4) {
    return false;
  }
  return disk_phase[phase];
}

bool Apple2Machine::diskQ6() const {
  return disk_q6;
}

bool Apple2Machine::diskQ7() const {
  return disk_q7;
}

uint8_t Apple2Machine::diskDataLatch() const {
  return disk_data_latch;
}

bool Apple2Machine::diskInserted(int drive) const {
  if(drive < 1 || drive > 2) {
    return false;
  }
  return disk_inserted[drive - 1];
}

int Apple2Machine::diskTrack() const {
  return disk_half_track / 2;
}

bool Apple2Machine::slotRomLoaded(int slot) const {
  if(slot < 1 || slot > 7) {
    return false;
  }
  return slot_rom_loaded[slot];
}

int Apple2Machine::displayRows() const {
  if(text_mode) {
    return 24;
  }
  return mixed_mode ? 44 : 48;
}

bool Apple2Machine::isDisplayTextRow(int displayRow) const {
  return text_mode || (mixed_mode && displayRow >= 40);
}

char Apple2Machine::displayCharAt(int displayRow, int col) const {
  if(isDisplayTextRow(displayRow)) {
    int textRow = text_mode ? displayRow : 20 + (displayRow - 40);
    return textCharAt(textRow, col);
  }
  return loresCharAt(displayRow, col);
}

uint16_t Apple2Machine::screenAddressForCell(int row, int col) const {
  return textBase() + (row & 0x07) * 0x80 + (row >> 3) * 0x28 + col;
}

char Apple2Machine::textCharAt(int row, int col) const {
  return appleTextChar(mem[screenAddressForCell(row, col)]);
}

char Apple2Machine::loresCharAt(int pixelRow, int col) const {
  int textRow = pixelRow / 2;
  return appleLoresChar(mem[screenAddressForCell(textRow, col)],
                        (pixelRow & 0x01) != 0);
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

uint8_t Apple2Machine::accessSlotIo(int addr) {
  int slot = ((addr & 0x00ff) >> 4) & 0x07;
  int switchNum = addr & 0x0f;
  slot_access_count[slot]++;

  if(slot == 6) {
    return accessDiskII(switchNum);
  }
  return 0x00;
}

uint8_t Apple2Machine::accessDiskII(int switchNum) {
  switch(switchNum & 0x0f) {
    case 0x0: updateDiskPhase(0, false); break;
    case 0x1: updateDiskPhase(0, true); break;
    case 0x2: updateDiskPhase(1, false); break;
    case 0x3: updateDiskPhase(1, true); break;
    case 0x4: updateDiskPhase(2, false); break;
    case 0x5: updateDiskPhase(2, true); break;
    case 0x6: updateDiskPhase(3, false); break;
    case 0x7: updateDiskPhase(3, true); break;
    case 0x8: disk_motor_on = false; break;
    case 0x9: disk_motor_on = true; break;
    case 0xA: disk_drive = 1; break;
    case 0xB: disk_drive = 2; break;
    case 0xC: disk_q6 = false; break;
    case 0xD: disk_q6 = true; break;
    case 0xE: disk_q7 = false; break;
    case 0xF: disk_q7 = true; break;
  }

  if((switchNum & 0x0f) == 0x0c && !disk_q6 && !disk_q7) {
    disk_data_latch = readDiskByte();
  }
  return disk_data_latch;
}

uint8_t Apple2Machine::readDiskByte() {
  if(!disk_motor_on || !disk_inserted[disk_drive - 1]) {
    return disk_data_latch;
  }

  int track = diskTrack();
  if(track < 0) {
    track = 0;
  } else if(track > 34) {
    track = 34;
  }

  size_t trackOffset = (size_t)track * 16 * 256;
  size_t pos = disk_stream_pos[disk_drive - 1] % (16 * 256);
  uint8_t value = disk_image[disk_drive - 1][trackOffset + pos];
  disk_stream_pos[disk_drive - 1] = (pos + 1) % (16 * 256);
  return value;
}

bool Apple2Machine::isProtectedRomAddress(int addr) const {
  int a = addr & 0xffff;
  if(rom_loaded && a >= rom_start) {
    return true;
  }
  if(a >= 0xc100 && a <= 0xc7ff) {
    int slot = (a >> 8) & 0x07;
    return slot_rom_loaded[slot];
  }
  return false;
}

void Apple2Machine::updateDiskPhase(int phase, bool on) {
  if(phase < 0 || phase >= 4) {
    return;
  }

  disk_phase[phase] = on;
  if(!on) {
    return;
  }

  int forward = (disk_last_phase + 1) & 0x03;
  int backward = (disk_last_phase + 3) & 0x03;
  if(phase == forward && disk_half_track < 68) {
    disk_half_track++;
    disk_stream_pos[0] = 0;
    disk_stream_pos[1] = 0;
  } else if(phase == backward && disk_half_track > 0) {
    disk_half_track--;
    disk_stream_pos[0] = 0;
    disk_stream_pos[1] = 0;
  }
  disk_last_phase = phase;
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
