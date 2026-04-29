#include "emu_core.h"

#include <cstdio>
#include <cstring>
#include <fstream>

static uint8_t mem[65536];

static int memoryReadCb(int addr, int size, uint8_t *dst) {
  if(dst != NULL) {
    memcpy(dst, mem + (addr & 0xffff), size);
  }
  return 0;
}

static int memoryWriteCb(int addr, int size, const uint8_t *src) {
  if(src != NULL) {
    memcpy(mem + (addr & 0xffff), src, size);
  }
  return 0;
}

static void printCpuState(const char *prefix, const MOS6502 &cpu) {
  printf("%s pc=%04x opcode=%02x A=%02x X=%02x Y=%02x SP=%02x "
         "C=%d Z=%d I=%d D=%d V=%d N=%d cycles=%d\n",
         prefix, cpu.reg_PC, mem[cpu.reg_PC], cpu.reg_A, cpu.reg_X,
         cpu.reg_Y, cpu.reg_SP, cpu.flag_C, cpu.flag_Z, cpu.flag_I,
         cpu.flag_D, cpu.flag_O, cpu.flag_N, cpu.cycle_counter);
}

int main() {
  std::ifstream rom("ROM/6502_functional_test.bin", std::ios::binary);
  if(!rom) {
    fprintf(stderr, "Failed to open ROM/6502_functional_test.bin\n");
    return 1;
  }

  rom.read((char *)mem, sizeof(mem));
  if(rom.gcount() != (std::streamsize)sizeof(mem)) {
    fprintf(stderr, "ROM/6502_functional_test.bin is incomplete\n");
    return 1;
  }

  MOS6502 cpu;
  cpu.setMemoryReadCallBack(memoryReadCb);
  cpu.setMemoryWriteCallBack(memoryWriteCb);
  cpu.reset();
  cpu.reg_PC = 0x0400;

  uint16_t last_pc = cpu.reg_PC;
  int same_pc_count = 0;
  for(unsigned long step = 0; step < 200000000; ++step) {
    cpu.step();

    if(cpu.reg_PC == 0x3469) {
      printCpuState("6502 functional test passed:", cpu);
      return 0;
    }

    if(cpu.reg_PC == last_pc) {
      same_pc_count++;
      if(same_pc_count > 20) {
        printCpuState("6502 functional test stuck:", cpu);
        return 1;
      }
    } else {
      last_pc = cpu.reg_PC;
      same_pc_count = 0;
    }
  }

  printCpuState("6502 functional test timed out:", cpu);
  return 1;
}
