# simple_6502_emulator
My simple practice to write a 6502 emulator in cpp.
Work-in-progress

## Build

```sh
make
```

The emulator binary is written to `build/emu`.

## Run

```sh
./build/emu
```

Press `Ctrl-C` to exit.

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
```

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
make test
```

This runs the bundled `ROM/6502_functional_test.bin` against the CPU core and
smoke-tests Woz Monitor, Integrated BASIC, and A1-Assembler.
