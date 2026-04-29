#include "emu_core.h"

#include <cstdio>
#include <cstring>
#include <fstream>
#include <queue>
#include <string>
#include <vector>

static uint8_t mem[65536];
static std::queue<uint8_t> input;
static std::string output;

static uint8_t normalizeInput(uint8_t c) {
  if(c == '\n') c = '\r';
  if(c == 0x7f) c = 0xdf;
  if(c >= 'a' && c <= 'z') c -= 32;
  return c | 0x80;
}

static int memoryReadCb(int addr, int size, uint8_t *dst) {
  for(int i = 0; i < size; ++i) {
    int a = (addr + i) & 0xffff;
    if(a == 0xd011) {
      dst[i] = input.empty() ? 0x00 : 0x80;
    } else if(a == 0xd010) {
      if(input.empty()) {
        dst[i] = 0x00;
      } else {
        dst[i] = input.front();
        input.pop();
      }
    } else if(a == 0xd012) {
      dst[i] = 0x00;
    } else {
      dst[i] = mem[a];
    }
  }
  return 0;
}

static int memoryWriteCb(int addr, int size, const uint8_t *src) {
  for(int i = 0; i < size; ++i) {
    int a = (addr + i) & 0xffff;
    mem[a] = src[i];
    if(a == 0xd012) {
      uint8_t ch = src[i] & 0x7f;
      if(ch == 0x7f) {
        mem[a] = 0x00;
        continue;
      }
      if(ch == '\r') ch = '\n';
      if(ch == (0xdf & 0x7f)) {
        output += "\b \b";
      } else {
        output.push_back((char)ch);
      }
      mem[a] = 0x00;
    }
  }
  return 0;
}

static bool loadRom(const char *path, uint16_t addr) {
  std::ifstream rom(path, std::ios::binary);
  if(!rom) {
    fprintf(stderr, "Failed to open %s\n", path);
    return false;
  }
  rom.read((char *)mem + addr, 65536 - addr);
  return true;
}

static void feed(const char *script) {
  while(*script) {
    input.push(normalizeInput((uint8_t)*script++));
  }
}

static bool containsAll(const std::vector<std::string> &needles) {
  for(size_t i = 0; i < needles.size(); ++i) {
    if(output.find(needles[i]) == std::string::npos) {
      return false;
    }
  }
  return true;
}

struct Stage {
  const char *script;
  std::vector<std::string> expected;
};

static bool runStages(const char *name, const std::vector<Stage> &stages) {
  memset(mem, 0, sizeof(mem));
  input = std::queue<uint8_t>();
  output.clear();

  if(!loadRom("ROM/Woz Monitor.rom", 0xff00) ||
     !loadRom("ROM/Integrated BASIC.hex", 0xe000) ||
     !loadRom("ROM/A1A90.BIN", 0x9000)) {
    return false;
  }

  MOS6502 cpu;
  cpu.setMemoryReadCallBack(memoryReadCb);
  cpu.setMemoryWriteCallBack(memoryWriteCb);
  cpu.reset();

  for(size_t i = 0; i < stages.size(); ++i) {
    feed(stages[i].script);

    bool stage_ok = false;
    for(unsigned long step = 0; step < 50000000; ++step) {
      cpu.step();
      if(input.empty() && containsAll(stages[i].expected)) {
        stage_ok = true;
        break;
      }
    }

    if(!stage_ok) {
      fprintf(stderr, "%s failed in stage %zu. Output was:\n%s\n",
              name, i + 1, output.c_str());
      return false;
    }
  }

  printf("%s passed\n", name);
  return true;
}

int main() {
  bool ok = true;

  ok &= runStages("Woz monitor",
                  {{"FF00.FF02\r",
                    {"\\\nFF00.FF02", "FF00: D8 58 A0"}},
                   {"0300:A9 C1 20 EF FF 4C 00 FF\r0300R\r",
                    {"0300: A9A\\"}},
                   {"FF0X" "\x7f" "0.FF02\r",
                    {"FF0X\b \b0.FF02", "FF00: D8 58 A0"}}});

  ok &= runStages("Integrated BASIC",
                  {{"E000R\r"
                    "PRINT \"HELLO\"\r"
                    "10 LET A=5\r"
                    "20 PRINT A\r"
                    "30 END\r"
                    "LIST\r"
                    "RUN\r"
                    "10 FOR I=1 TO 5\r"
                    "20 PRINT I\r"
                    "30 NEXT I\r"
                    "40 END\r"
                    "RUN\r",
                    {"HELLO", "   10 LET A=5", "\n5\n", "\n1\n2\n3\n4\n5\n"}}});

  ok &= runStages("A1-Assembler",
                  {{"9000R\r"
                    "1000 ECHO .EQ $FFEF\r"
                    "1010 START\r"
                    "1020    LDA #\"A\"\r"
                    "1030    JSR ECHO\r"
                    "1040    RTS\r"
                    "SBASM\r",
                    {"A1-ASM V1.0", "$0280.$0285", "0 ERRORS"}},
                   {"XEC START\r",
                    {"XEC START\nA\n%"}}});

  return ok ? 0 : 1;
}
