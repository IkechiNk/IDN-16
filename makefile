CC = gcc
CFLAGS = -Wall -Wextra -Wshadow -Wformat=2 -fsanitize=address,undefined -Iinclude -Isrc

# Core emulator
CORE_SRC = src/core/cpu.c src/core/memory.c src/core/decoder.c src/core/instructions.c
CORE_BIN = idn16

# Tools
ASM_SRC = src/tools/assembler.c src/tools/lexer.c src/tools/parser.c src/tools/codegen.c src/tools/symbols.c src/core/instructions.c src/core/memory.c
ASM_OBJS = $(ASM_SRC:.c=.o)
ASM_BIN = asmblr
DASM_SRC = src/tools/disassembler.c
DASM_OBJS = $(DASM_SRC:.c=.o)
DASM_BIN = dasmblr

# Tests
UNITY_SRC = src/tests/Unity/unity.c
TEST_MEM_SRC = src/tests/test_memory.c
TEST_MEM_BIN = test_memory
TEST_CPU_SRC = src/tests/test_cpu.c
TEST_CPU_BIN = test_cpu

.PHONY: all core assembler test run_test_memory run_test_cpu clean

all: core assembler test

core: $(CORE_BIN)

$(CORE_BIN): $(CORE_SRC)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

assembler: $(ASM_BIN)

$(ASM_BIN): $(ASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

disassembler: $(DASM_BIN)

$(DASM_BIN): $(DASM_OBJS)
	$(CC) $(CFLAGS) -o $@ $^

src/tools/%.o: src/tools/%.c
	$(CC) $(CFLAGS) -c $< -o $@

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
	rm -f $(CORE_BIN) $(ASM_BIN) $(TEST_MEM_BIN) $(TEST_CPU_BIN) src/core/*.o src/tools/*.o *.bin