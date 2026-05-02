#ifndef APPLE2_MACHINE_H
#define APPLE2_MACHINE_H

#include "emu_core.h"

#include <cstdint>
#include <queue>
#include <string>
#include <vector>

class Apple2Machine {
  public:
    Apple2Machine();

    void reset();
    void resetCpu();
    void step();

    bool loadRom(const std::string &path, uint16_t addr);
    bool loadSystemRom(const std::string &path);
    bool loadSlotRom(const std::string &path, int slot);
    bool loadDiskImage(const std::string &path, int drive);
    void installDemoRom();
    void installDiskIIRomScaffold();

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
    unsigned int slotAccessCount(int slot) const;
    bool diskMotorOn() const;
    int diskDrive() const;
    bool diskPhaseOn(int phase) const;
    bool diskQ6() const;
    bool diskQ7() const;
    uint8_t diskDataLatch() const;
    bool diskInserted(int drive) const;
    int diskTrack() const;
    bool slotRomLoaded(int slot) const;

    int displayRows() const;
    bool isDisplayTextRow(int displayRow) const;
    char displayCharAt(int displayRow, int col) const;
    uint16_t screenAddressForCell(int row, int col) const;
    char textCharAt(int row, int col) const;
    char loresCharAt(int pixelRow, int col) const;
    uint8_t screenByteAt(int row, int col) const;

  private:
    uint16_t textBase() const;
    char appleTextChar(uint8_t value) const;
    char appleLoresChar(uint8_t value, bool upperNibble) const;
    void readSoftSwitch(int addr);
    void writeSoftSwitch(int addr);
    uint8_t accessSlotIo(int addr);
    uint8_t accessDiskII(int switchNum);
    uint8_t readDiskByte();
    bool isProtectedRomAddress(int addr) const;
    void updateDiskPhase(int phase, bool on);
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
    bool rom_loaded;
    uint16_t rom_start;
    bool slot_rom_loaded[8];
    unsigned int slot_access_count[8];
    bool disk_phase[4];
    bool disk_motor_on;
    int disk_drive;
    bool disk_q6;
    bool disk_q7;
    uint8_t disk_data_latch;
    bool disk_inserted[2];
    std::vector<uint8_t> disk_image[2];
    size_t disk_stream_pos[2];
    int disk_half_track;
    int disk_last_phase;
};

#endif
