# simple_6502_emulator
My simple practice to write a 6502 emulator in cpp.
Work-in-progress

## Build

```sh
make
```

The emulator binary is written to `build/emu`.

To build the experimental Apple II command-line frontend:

```sh
make apple2_cli
```

## Run

```sh
./build/emu
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
./build/apple2_cli
```

This is an early Apple II text-mode scaffold that works in a plain terminal. It
implements:

- an `Apple2Machine` core separated from the command-line frontend
- 40x24 text page 1 rendering from `$0400-$07ff`
- Apple II text page row addressing
- keyboard latch at `$c000`
- keyboard strobe clear at `$c010`
- display soft switches `$c050-$c057`
- command-line lo-res rendering approximation
- a built-in demo ROM that writes to the text page and echoes typed keys

Press `Ctrl-C` to exit.

You can also try booting an external ROM image at `$d000`:

```sh
./build/apple2_cli --rom path/to/apple2.rom
```

Full Apple II ROMs, hi-res artifact color, speaker, slots, and Disk II are not
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
make test
```

This runs the bundled `ROM/6502_functional_test.bin` against the CPU core,
smoke-tests Woz Monitor, Integrated BASIC, and A1-Assembler, and runs the
Apple II CLI text/keyboard/soft-switch smoke test.
