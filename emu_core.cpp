#include "opcodes.h"
#include "emu_core.h"

MOS6502::MOS6502(void) {
  memoryReadCallBack = NULL;
  memoryWriteCallBack = NULL;
}

MOS6502::~MOS6502() {
}

int MOS6502::setMemoryReadCallBack(int (*cb)(int, int, uint8_t *)) {
  if(cb == NULL) return -1;
  memoryReadCallBack = cb;
  return 0;
}
int MOS6502::setMemoryWriteCallBack(int (*cb)(int, int, const uint8_t *)) {
  if(cb == NULL) return -1;
  memoryWriteCallBack = cb;
  return 0;
}

void MOS6502::reset() {
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
  reg_PC = ((uint16_t)readByte(0xfffd) << 8) | readByte(0xfffc);
}

void MOS6502::step() {
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

void MOS6502::cycle(int t) {
  cycle_counter += t;
}

uint8_t MOS6502::logicShiftRight(uint8_t origin) {
  flag_C = origin & 0x01;
  return origin >> 1;
}

uint8_t MOS6502::readByte(uint16_t addr) {
  uint8_t data = 0;
  if(memoryReadCallBack != NULL) {
    memoryReadCallBack(addr, 1, &data);
    pin_RW = 1; //Read
    addr_bus = addr;
  } else fprintf(stderr, "MOS6502: Error reading memory.\n");
  return data;
}

void MOS6502::writeByte(uint8_t data, uint16_t addr) {
  if(memoryWriteCallBack != NULL) {
    memoryWriteCallBack(addr, 1, &data);
    pin_RW = 0; //Write
    addr_bus = addr;
  } else fprintf(stderr, "MOS6502: Error writing memory.\n");
}

uint16_t MOS6502::readWord(uint16_t addr) {
  uint8_t dataL = readByte(addr);
  uint8_t dataH = readByte((uint16_t)(addr + 1));
  return ((uint16_t)dataH << 8) | dataL;
}

uint16_t MOS6502::readWordZeroPage(uint8_t addr) {
  uint8_t dataL = readByte(addr);
  uint8_t dataH = readByte((uint8_t)(addr + 1));
  return ((uint16_t)dataH << 8) | dataL;
}

uint16_t MOS6502::readWordPageWrap(uint16_t addr) {
  uint16_t next_addr = (addr & 0xff00) | ((addr + 1) & 0x00ff);
  uint8_t dataL = readByte(addr);
  uint8_t dataH = readByte(next_addr);
  return ((uint16_t)dataH << 8) | dataL;
}

void MOS6502::writeWord(uint16_t data, uint16_t addr) {
  writeByte((uint8_t)(data & 0xff), addr);
  writeByte((uint8_t)((data >> 8) & 0xff), (uint16_t)(addr + 1));
}


void MOS6502::checkNZ(uint8_t value) {
  if(value == 0) flag_Z = 1;
    else flag_Z = 0;
  if((value & 0x80) == 0x80) flag_N = 1;
    else flag_N = 0;
}

void MOS6502::stackPush(uint8_t value) {
  writeByte(value, (uint16_t)(reg_SP | 0x0100));
  if(reg_SP >= 0x01) reg_SP--;
  else {
    reg_SP = 0xff;
    //cout << "Warnning: Stack Overflow" << endl;
  }//Stack Overflow
}

uint8_t MOS6502::stackPop(void) {
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

void MOS6502::add(uint8_t value) {
  unsigned const binary_sum = reg_A + value + flag_C;
  flag_O = (~(reg_A ^ value) & (reg_A ^ binary_sum) & 0x80) != 0;

  if(flag_D) {
    unsigned bcd_sum = binary_sum;
    if(((reg_A & 0x0f) + (value & 0x0f) + flag_C) > 9) bcd_sum += 0x06;
    if(bcd_sum > 0x99) bcd_sum += 0x60;
    flag_C = bcd_sum > 0xff;
    reg_A = (uint8_t)bcd_sum;
  } else {
    flag_C = binary_sum > 0xff;
    reg_A = (uint8_t)binary_sum;
  }

  checkNZ(reg_A);
}

void MOS6502::sub(uint8_t value) {
  unsigned const borrow = flag_C ? 0 : 1;
  unsigned const binary_diff = reg_A - value - borrow;
  flag_C = binary_diff < 0x100;
  flag_O = ((reg_A ^ value) & (reg_A ^ binary_diff) & 0x80) != 0;

  if(flag_D) {
    unsigned bcd_diff = binary_diff;
    if((reg_A & 0x0f) < ((value & 0x0f) + borrow)) bcd_diff -= 0x06;
    if(binary_diff > 0x99) bcd_diff -= 0x60;
    reg_A = (uint8_t)bcd_diff;
  } else {
    reg_A = (uint8_t)binary_diff;
  }

  checkNZ(reg_A);
}

void MOS6502::compare(uint16_t reg, uint16_t num) {
  uint16_t tmp = reg - (uint8_t)(num&0xff);
  flag_C = (tmp < 0x100);
  flag_N = (tmp & 0x80) != 0;
  flag_Z = !(tmp & 0xFF);
}
