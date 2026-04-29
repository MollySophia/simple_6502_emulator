CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic

BUILD_DIR := build
EMU := $(BUILD_DIR)/emu
APPLE2_CLI := $(BUILD_DIR)/apple2_cli
FUNCTIONAL_TEST := $(BUILD_DIR)/functional_test
APPLE1_SMOKE_TEST := $(BUILD_DIR)/apple1_smoke_test

EMU_SRC := emu_main.cpp emu_core.cpp console.cpp
APPLE2_CLI_SRC := apple2_cli.cpp apple2_machine.cpp emu_core.cpp
FUNCTIONAL_TEST_SRC := tests/functional_test.cpp emu_core.cpp console.cpp
APPLE1_SMOKE_TEST_SRC := tests/apple1_smoke_test.cpp emu_core.cpp console.cpp
HEADERS := apple2_machine.h emu_core.h console.h opcodes.h $(wildcard opcode_handler/*.h)

.PHONY: all apple2_cli test clean

all: $(EMU)

apple2_cli: $(APPLE2_CLI)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EMU): $(EMU_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(EMU_SRC) -o $(EMU)

$(APPLE2_CLI): $(APPLE2_CLI_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(APPLE2_CLI_SRC) -o $(APPLE2_CLI)

$(FUNCTIONAL_TEST): $(FUNCTIONAL_TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(FUNCTIONAL_TEST_SRC) -o $(FUNCTIONAL_TEST)

$(APPLE1_SMOKE_TEST): $(APPLE1_SMOKE_TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(APPLE1_SMOKE_TEST_SRC) -o $(APPLE1_SMOKE_TEST)

test: $(FUNCTIONAL_TEST) $(APPLE1_SMOKE_TEST) $(APPLE2_CLI)
	./$(FUNCTIONAL_TEST)
	./$(APPLE1_SMOKE_TEST)
	./$(APPLE2_CLI) --smoke-test

clean:
	rm -rf $(BUILD_DIR)
