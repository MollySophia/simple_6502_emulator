#ifndef _EMU_CORE_
#define _EMU_CORE_

#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
using namespace std;

class Emulator {
  public:
    Emulator();
    ~Emulator();
    int reset();
    int loadRom(char *filename, uint16_t addr);
    void readMem(char *buf, uint16_t addr, int len);
    uint8_t readByte(uint16_t addr);
    void writeByte(uint8_t data, uint16_t addr);
    uint16_t readWord(uint16_t addr);
    void writeWord(uint16_t data, uint16_t addr);

    uint8_t LogicShiftRight(uint8_t origin);
    void check_N_Z(uint8_t value);
    void stack_push(uint8_t value);
    uint8_t stack_pop(void);
    void add(uint8_t value);
    void sub(uint8_t value);
    void compare(uint16_t reg, uint16_t num);

    void cycle(int t);
    void IR_step();

    uint8_t data_bus;
    uint16_t addr_buf;
    uint16_t addr_bus;
    int cycle_counter;
    uint8_t reg_A;  // Accumulator Register
    uint8_t reg_X;  // Index Register (X and Y)
    uint8_t reg_Y;
    uint16_t reg_PC; // Program Counter Register
    uint8_t reg_SP; // Stack Pointer
    uint8_t reg_IR; // Instruction Register
    FILE *fp;
    bool flag_exit;

    bool flag_C; // Carry Flag
    bool flag_Z; // Zero Flag
    bool flag_I; // Interrupt Disable
    bool flag_D; // Decimal Mode
    bool flag_B; // Break
    bool flag_O; // Overflow
    bool flag_N; // Negative

    bool pin_RW; // Read/Write Pin 1:read 0:write
    uint8_t *mem;   // RAM
    int counter;
};

#endif
