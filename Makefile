CXX ?= g++
CXXFLAGS ?= -std=c++11 -Wall -Wextra -pedantic

BUILD_DIR := build
EMU := $(BUILD_DIR)/emu
FUNCTIONAL_TEST := $(BUILD_DIR)/functional_test
APPLE1_SMOKE_TEST := $(BUILD_DIR)/apple1_smoke_test

EMU_SRC := emu_main.cpp emu_core.cpp console.cpp
FUNCTIONAL_TEST_SRC := tests/functional_test.cpp emu_core.cpp console.cpp
APPLE1_SMOKE_TEST_SRC := tests/apple1_smoke_test.cpp emu_core.cpp console.cpp
HEADERS := emu_core.h console.h opcodes.h $(wildcard opcode_handler/*.h)

.PHONY: all test clean

all: $(EMU)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(EMU): $(EMU_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) $(EMU_SRC) -o $(EMU)

$(FUNCTIONAL_TEST): $(FUNCTIONAL_TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(FUNCTIONAL_TEST_SRC) -o $(FUNCTIONAL_TEST)

$(APPLE1_SMOKE_TEST): $(APPLE1_SMOKE_TEST_SRC) $(HEADERS) | $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -I. $(APPLE1_SMOKE_TEST_SRC) -o $(APPLE1_SMOKE_TEST)

test: $(FUNCTIONAL_TEST) $(APPLE1_SMOKE_TEST)
	./$(FUNCTIONAL_TEST)
	./$(APPLE1_SMOKE_TEST)

clean:
	rm -rf $(BUILD_DIR)
