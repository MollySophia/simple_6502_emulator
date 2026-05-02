# simple_6502_emulator
My simple practice to write a 6502 emulator in cpp.
Work-in-progress

## Build

```sh
cmake -S . -B build
cmake --build build
```

The emulator binary is written to `build/bin/emu`.

To build the experimental Apple II command-line frontend:

```sh
cmake --build build --target apple2_cli
```

The Apple II CLI binary is written to `build/bin/apple2_cli`.

On Windows, run the commands from a Visual Studio Developer Command Prompt or
initialize the toolchain first, for example:

```cmd
call "C:\Program Files\Microsoft Visual Studio\18\Community\VC\Auxiliary\Build\vcvarsall.bat" arm64
cmake -S . -B build-msvc-arm64 -G "NMake Makefiles"
cmake --build build-msvc-arm64
```

The Windows ARM64 binaries are written to `build-msvc-arm64\bin`.

## Run

```sh
./build/bin/emu
```

On Windows with the build directory from the example above:

```cmd
build-msvc-arm64\bin\emu.exe
```

Press `Ctrl-C` to exit.

The built-in demo also has host-side convenience keys:

- `G`: switch to lo-res graphics and fill the text/graphics page with a pattern
- `T`: switch back to text and restore the demo title

The emulator currently boots into an Apple-1 style Woz Monitor. Some useful
commands to try:

```text
FF00.FF10
0300:A9 C1 20 EF FF 4C 00 FF
0300R
```

Start Integrated BASIC:

```text
E000R
PRINT "HELLO"
10 LET A=5
20 PRINT A
30 END
RUN
10 FOR I=1 TO 5
20 PRINT I
30 NEXT I
40 END
RUN
10 DIM A$(10)
20 LET A$="APPLE"
30 PRINT A$
40 PRINT A$(3)
50 END
RUN
```

Apple BASIC treats any keypress during program execution as an interrupt, so
when pasting commands interactively, wait for the `>` prompt after `RUN` before
pasting the next command.

## Apple II CLI

```sh
./build/bin/apple2_cli [--rom path/to/apple2.rom] [--disk1 path/to/disk.dsk] [--disk2 path/to/disk.dsk] [--slot6-rom path/to/rom.bin]
```

On Windows with the build directory from the example above:

```cmd
build-msvc-arm64\bin\apple2_cli.exe [--rom path\to\apple2.rom] [--disk1 path\to\disk.dsk] [--disk2 path\to\disk.dsk] [--slot6-rom path\to\rom.bin]
```

This is an early Apple II text-mode scaffold that works in a plain terminal. It
implements:

- an `Apple2Machine` core separated from the command-line frontend
- 40x24 text rendering from page 1 (`$0400-$07ff`) or page 2 (`$0800-$0bff`)
- Apple II text page row addressing
- keyboard latch at `$c000`
- keyboard strobe clear at `$c010`
- display soft switches `$c050-$c057`
- command-line lo-res rendering as 40x48 text cells
- mixed mode with 40 lo-res rows and the bottom 4 text rows
- slot I/O dispatch for `$c080-$c0ff`
- a minimal slot 6 Disk II soft-switch scaffold
- slot ROM mapping for `$c100-$c7ff`, including slot 6 at `$c600-$c6ff`
- loading 140 KiB DOS-order `.dsk` images into drive 1 or drive 2
- a simple current-track disk byte stream for early controller experiments
- a built-in demo ROM that writes to the text page and echoes typed keys

Press `Ctrl-C` to exit.

You can also try booting an external Apple II system ROM image and inserting
disk images. ROM images up to 12 KiB are loaded into the high ROM area ending at
`$ffff`; a 12 KiB image maps to `$d000-$ffff`, an 8 KiB image maps to
`$e000-$ffff`, and a 4 KiB image maps to `$f000-$ffff`. The reset vector is then
read from the loaded ROM:

```sh
./build/bin/apple2_cli --rom path/to/apple2.rom --disk1 path/to/dos33.dsk
```

For debugging the slot 6 path without an Apple Disk II ROM, you can install the
built-in scaffold ROM. It maps a tiny test ROM at `$c600`, turns on the Disk II
motor, reads one byte from the current disk stream, writes it to text row 1, and
prints a short message on row 0:

```sh
./build/bin/apple2_cli --disk1 path/to/dos33.dsk --disk-rom-scaffold
```

The loaded system ROM area is read-only to emulated code. Slot 6 currently
tracks Disk II phase, motor, drive, and Q6/Q7 soft-switch state. The current
disk data path reads bytes from the selected 140 KiB image track in DOS sector
order; it is not yet a full Disk II 6-and-2 nibble stream and does not write disk
images. Hi-res artifact color, speaker, and full Disk II media emulation are not
implemented yet.

Start A1-Assembler:

```text
9000R
1000 ECHO .EQ $FFEF
1010 START
1020    LDA #"A"
1030    JSR ECHO
1040    RTS
SBASM
XEC START
```

## Test

```sh
ctest --test-dir build --output-on-failure
```

This runs the bundled `ROM/6502_functional_test.bin` against the CPU core and
smoke-tests Woz Monitor, Integrated BASIC, and A1-Assembler. When `apple2_cli`
is built, it also runs the Apple II CLI text/keyboard/soft-switch/slot/system-ROM
smoke test.
