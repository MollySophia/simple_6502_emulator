#ifndef _EMU_CORE_
#define _EMU_CORE_

#include <iostream>
#include <cstdio>
#include <cstring>
#include <fstream>
using namespace std;

class MOS6502 {
  public:
    MOS6502();
    ~MOS6502();
    void reset();

    int setMemoryReadCallBack(int (*cb)(int, int, uint8_t *));
    int setMemoryWriteCallBack(int (*cb)(int, int, const uint8_t *));

    void cycle(int t);
    void step();

    uint8_t data_bus;
    uint16_t addr_bus;
    uint16_t addr_buf;
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
    int counter;

  private:
    /* cpu operations */
    uint8_t logicShiftRight(uint8_t origin);
    void checkNZ(uint8_t value);
    void stackPush(uint8_t value);
    uint8_t stackPop(void);
    void add(uint8_t value);
    void sub(uint8_t value);
    void compare(uint16_t reg, uint16_t num);

    /* int memoryReadCallBack(int addr, int size, uint8_t * dst) */
    int (*memoryReadCallBack)(int, int, uint8_t *);

    /* int memoryWriteCallBack(int addr, int size, const uint8_t * src) */
    int (*memoryWriteCallBack)(int, int, const uint8_t *);

    uint8_t readByte(uint16_t addr);
    uint16_t readWord(uint16_t addr);
    void writeByte(uint8_t data, uint16_t addr);
    void writeWord(uint16_t data, uint16_t addr);
};

#endif
