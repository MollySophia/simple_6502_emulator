#include "opcodes.h"
#include "emu_core.h"
#include "console.h"
#include <unistd.h>

Emulator::Emulator(void) {
  mem = new uint8_t[65536];
  reset();
}

Emulator::~Emulator() {
  delete mem;
}

int Emulator::reset() {
  counter = 0;
  cycle_counter = 0;
  reg_A = 0;
  reg_X = 0;
  reg_Y = 0;
  reg_SP = 0xfd;
  flag_B = 0;
  flag_C = 0;
  flag_D = 0;
  flag_I = 0;
  flag_N = 0;
  flag_O = 0;
  flag_Z = 0;
  data_bus = 0;
  addr_bus = 0;
  memset(mem, 0, sizeof(mem));

  loadRom((char*)"ROM/Woz Monitor.rom", 0xFF00);
  loadRom((char*)"ROM/Integrated BASIC.hex", 0xE000);
  loadRom((char*)"ROM/A1A90.BIN", 0x9000);

  reg_PC = ((uint16_t)readByte(0xfffd) << 8) | readByte(0xfffc);
}

void Emulator::IR_step() {
  reg_IR = readByte(reg_PC++);
  // if(cycle_counter >= 260000) {
  //   fprintf(fp, "\nAddr:%x Instruction:%2x ", (int)reg_PC - 1, (int)reg_IR);
  //   switch (reg_IR) {
  //     #include "opcode_print.h"
  //   }
  //   fprintf(fp, "\n");
  // }
  cycle(2);
  switch(reg_IR) {
    case CLC: flag_C = 0;break; // Clear Carry
    case SEC: flag_C = 1;break; // Set Carry
    case CLI: flag_I = 0;break; // Clear Interrupt
    case SEI: flag_I = 1;break; // Set Interrupt
    case CLV: flag_O = 0;break; // Clear Overflow
    case CLD: flag_D = 0;break; // Clear Decimal
    case SED: flag_D = 1;break; // Set Decimal
    case NOP:break; //NOP

    #include "opcode_handler/jump.h"
    #include "opcode_handler/load.h"
    #include "opcode_handler/shift.h"
    #include "opcode_handler/stack.h"
    #include "opcode_handler/register.h"
    #include "opcode_handler/store.h"
    #include "opcode_handler/compare.h"
    #include "opcode_handler/inc_dec.h"
    #include "opcode_handler/arithmetic.h"
    #include "opcode_handler/logical.h"
    #include "opcode_handler/branch.h"
    #include "opcode_handler/interrupts.h"
    default: {
      printf("Error: Undefined Instruction: %x\n", reg_PC);
    }
  }
}

void Emulator::cycle(int t) {
  usleep(t);
}

uint8_t Emulator::LogicShiftRight(uint8_t origin) {
  flag_C = origin & 0x01;
  return origin >> 1;
}

void Emulator::readMem(char *buf, uint16_t addr, int len) {
  memcpy(buf, mem + addr, len*sizeof(char));
  addr_bus = addr;
}

uint8_t Emulator::readByte(uint16_t addr) {
  pin_RW = 1;
  addr_bus = addr;
  return mem[addr];
}

void Emulator::writeByte(uint8_t data, uint16_t addr) {
  pin_RW = 0;
  addr_bus = addr;
  mem[addr] = data;
}

uint16_t Emulator::readWord(uint16_t addr) {
  pin_RW = 1;
  addr_bus = addr;
  return (mem[addr + 1] << 8) | mem[addr];
}

void Emulator::writeWord(uint16_t data, uint16_t addr) {
  mem[addr] = (uint8_t)(data & 0x0F);
  mem[addr + 1] = (uint8_t)((data & 0xF0) >> 8);
  addr_bus = addr;
  pin_RW = 0;
}

int Emulator::loadRom(char *filename, uint16_t addr) {
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
  romfile.read((char*)(mem + addr), len);
  romfile.close();
}

void Emulator::check_N_Z(uint8_t value) {
  if(value == 0) flag_Z = 1;
    else flag_Z = 0;
  if((value & 0x80) == 0x80) flag_N = 1;
    else flag_N = 0;
}

void Emulator::stack_push(uint8_t value) {
  writeByte(value, (uint16_t)(reg_SP | 0x0100));
  if(reg_SP >= 0x01) reg_SP--;
  else {
    reg_SP = 0xff;
    //cout << "Warnning: Stack Overflow" << endl;
  }//Stack Overflow
}

uint8_t Emulator::stack_pop(void) {
  if(reg_SP <= 0xfe) {
    reg_SP++;
    data_bus = readByte((uint16_t)(reg_SP | 0x0100));
    return data_bus;
  }
  else { //Stack Underflow
    reg_SP = 0x00;
    //cout << "Warning: Stack Underflow" << endl;
    return readByte(0x0100);
  }
}

void Emulator::add(uint8_t value) {
  unsigned const sum = reg_A + value + flag_C;
  flag_C = sum > 0xff;
  flag_O = (~(reg_A ^ value) & (reg_A ^ sum) & 0x80) >> 7;
  reg_A = (uint8_t)sum;
  check_N_Z(reg_A);
}

void Emulator::sub(uint8_t value) {
  add(~value);
}

void Emulator::compare(uint16_t reg, uint16_t num) {
  uint16_t tmp = reg - (uint8_t)(num&0xff);
  flag_C = (tmp < 0x100);
  flag_N = (tmp & 0x80) != 0;
  flag_Z = !(tmp & 0xFF);
}
