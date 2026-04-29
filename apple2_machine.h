#ifndef APPLE2_MACHINE_H
#define APPLE2_MACHINE_H

#include "emu_core.h"

#include <cstdint>
#include <queue>
#include <string>

class Apple2Machine {
  public:
    Apple2Machine();

    void reset();
    void resetCpu();
    void step();

    bool loadRom(const std::string &path, uint16_t addr);
    void installDemoRom();

    int memoryRead(int addr, int size, uint8_t *dst);
    int memoryWrite(int addr, int size, const uint8_t *src);

    void pushKey(uint8_t c);
    void handleHostKey(uint8_t c);

    bool isScreenDirty() const;
    void clearScreenDirty();

    bool isTextMode() const;
    bool isMixedMode() const;
    bool isPage2Mode() const;
    bool isHiresMode() const;

    uint16_t screenAddressForCell(int row, int col) const;
    char textCharAt(int row, int col) const;
    char loresCharAt(int row, int col) const;
    uint8_t screenByteAt(int row, int col) const;

  private:
    uint16_t textBase() const;
    char appleTextChar(uint8_t value) const;
    char appleLoresChar(uint8_t value, bool upperNibble) const;
    void readSoftSwitch(int addr);
    void writeSoftSwitch(int addr);
    void writeDemoTitle();
    void fillLoresDemoPattern();

    MOS6502 cpu;
    uint8_t mem[65536];
    std::queue<uint8_t> key_queue;
    uint8_t key_latch;
    bool key_strobe;
    bool screen_dirty;
    bool text_mode;
    bool mixed_mode;
    bool page2_mode;
    bool hires_mode;
};

#endif
