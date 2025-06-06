CC      = gcc
CFLAGS  = -Wall -Wextra -Wshadow -Wformat=2 -Wno-unused-function -Iinclude -Isrc
LFLAGS  = -Wno-error=implicit-function-declaration -Wno-error=unused-function
DEVFLAGS= -Wall -Wextra -Wshadow -Wformat=2 -fsanitize=address,undefined -Iinclude -Isrc

# --- Core Emulator ---
CORE_SRC = src/core/runner.c src/core/cpu.c src/core/memory.c src/core/instructions.c src/core/IO/display.c src/core/IO/keyboard.c
CORE_BIN = idn16

# --- Disassembler ---
DASM_SRC = src/tools/disassembler.c
DASM_INC = src/tools/dasm.c src/tools/dasm.h
DASM_BIN = dasm

# --- Assembler (Flex/Bison) ---
LEX	= flex
YACC = bison
PARSER   = src/tools/assembler/parser.y
PARSER_TABH = src/tools/assembler/parser.tab.h
PARSER_TABC = src/tools/assembler/parser.tab.c
LEXER = src/tools/assembler/lexer.l
LEXYY = src/tools/assembler/lex.yy.c
ASM_BIN  = asm

ASM_SRC  = src/tools/assembler/asm_main.c \
		   src/tools/assembler/codegen.c \
		   src/tools/assembler/symbol_table.c

# --- Unit Tests ---
TEST_DIR        = src/tests
TEST_MEM_SRC    = $(TEST_DIR)/test_memory.c
TEST_CPU_SRC    = $(TEST_DIR)/test_cpu.c
TEST_SYMTAB_SRC = $(TEST_DIR)/test_symbol_table.c
TEST_DASM_SRC   = $(TEST_DIR)/test_disassembler.c
TEST_CODEGEN_SRC= $(TEST_DIR)/test_codegen.c
UNITY_SRC       = $(TEST_DIR)/Unity/unity.c
TEST_MEM_BIN    = test_memory
TEST_CPU_BIN    = test_cpu
TEST_SYMTAB_BIN = test_symbol_table
TEST_DASM_BIN   = test_disassembler
TEST_CODEGEN_BIN= test_codegen

.PHONY: all core asmblr disasmblr test run_test_memory run_test_cpu run_test_symbol_table run_test_disassembler run_test_codegen clean

all: core test asmblr

# --- Core Emulator ---
core: $(CORE_BIN)

$(CORE_BIN): $(CORE_SRC) $(DASM_INC)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2 -lSDL2_ttf

# --- Assembler ---
asmblr: $(ASM_BIN)

$(PARSER_TABC) $(PARSER_TABH): $(PARSER)
	$(YACC) -Wcex -d -o $(PARSER_TABC) $(PARSER)

$(LEXYY): $(LEXER) $(PARSER_TABH)
	$(LEX) -o $(LEXYY) $(LEXER)

$(ASM_BIN): $(LEXYY) $(PARSER_TABC) $(PARSER_TABH) $(ASM_SRC)
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $(LEXYY) $(PARSER_TABC) $(ASM_SRC) -lfl

# --- Disassembler ---
disasmblr: $(DASM_BIN)

$(DASM_BIN): $(DASM_SRC) $(DASM_INC)
	$(CC) $(CFLAGS) -o $@ $^

# --- Object file rule for tools (optional, for future use) ---
src/tools/%.o: src/tools/%.c
	$(CC) $(CFLAGS) -c $< -o $@

# --- Unit Tests ---
test: run_test_memory run_test_cpu run_test_symbol_table run_test_disassembler run_test_codegen

run_test_memory: $(TEST_MEM_BIN)
	./$(TEST_MEM_BIN)

run_test_cpu: $(TEST_CPU_BIN)
	./$(TEST_CPU_BIN)

run_test_symbol_table: $(TEST_SYMTAB_BIN)
	./$(TEST_SYMTAB_BIN)

run_test_disassembler: $(TEST_DASM_BIN)
	./$(TEST_DASM_BIN)

run_test_codegen: $(TEST_CODEGEN_BIN)
	./$(TEST_CODEGEN_BIN)

$(TEST_MEM_BIN): $(TEST_MEM_SRC) $(UNITY_SRC) src/core/memory.c src/core/instructions.c
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(TEST_CPU_BIN): $(TEST_CPU_SRC) $(UNITY_SRC) src/core/cpu.c src/core/memory.c src/core/instructions.c $(DASM_INC)
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(TEST_SYMTAB_BIN): $(TEST_SYMTAB_SRC) $(UNITY_SRC) src/tools/assembler/symbol_table.c
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_DASM_BIN): $(TEST_DASM_SRC) $(UNITY_SRC) src/tools/dasm.c
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_CODEGEN_BIN): $(TEST_CODEGEN_SRC) $(UNITY_SRC) src/tools/assembler/codegen.c src/tools/assembler/symbol_table.c
	$(CC) $(CFLAGS) -o $@ $^

# --- Clean ---
clean:
	rm -f $(CORE_BIN) $(ASM_BIN) $(DASM_BIN) $(TEST_MEM_BIN) $(TEST_CPU_BIN) $(TEST_SYMTAB_BIN) $(TEST_DASM_BIN) $(TEST_CODEGEN_BIN)
	rm -f src/core/*.o src/tools/*.o src/tools/assembler/*.o
	rm -f $(LEXYY) $(PARSER_TABC) $(PARSER_TABH)
	rm -f *.o *.bin

# --- Install/Development helpers ---
install-deps:
	sudo apt update
	sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev flex bison git

dev: clean all