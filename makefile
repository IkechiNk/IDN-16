CC = gcc
CFLAGS = -Wall -Wextra -Iinclude -Isrc/core -Isrc/tests

# Core emulator sources and binary
CORE_SRC = src/core/cpu.c src/core/memory.c src/core/decoder.c src/core/instructions.c
CORE_BIN = idn16

# Test sources and binaries
UNITY_SRC = src/tests/Unity/unity.c
TEST_MEM_SRC = src/tests/test_memory.c
TEST_MEM_BIN = test_memory
TEST_CPU_SRC = src/tests/test_cpu.c
TEST_CPU_BIN = test_cpu

.PHONY: test run_test_memory run_test_cpu clean

all: core

core: $(CORE_BIN)

$(CORE_BIN): $(CORE_SRC)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

test: run_test_memory run_test_cpu

run_test_memory: $(TEST_MEM_BIN)
	./$(TEST_MEM_BIN)

run_test_cpu: $(TEST_CPU_BIN)
	./$(TEST_CPU_BIN)

$(TEST_MEM_BIN): $(TEST_MEM_SRC) $(UNITY_SRC) src/core/memory.c src/core/instructions.c
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(TEST_CPU_BIN): $(TEST_CPU_SRC) $(UNITY_SRC) src/core/cpu.c src/core/memory.c src/core/instructions.c
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

clean:
	rm -f $(CORE_BIN) $(TEST_MEM_BIN) $(TEST_CPU_BIN)