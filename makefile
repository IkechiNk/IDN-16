# IDN-16 Emulator Makefile
CC      = gcc
CFLAGS  = -Wall -Wextra -Wshadow -Wformat=2 -Wno-unused-function -Iinclude -Ilib/clay
LFLAGS  = -Wno-error=implicit-function-declaration -Wno-error=unused-function
DEVFLAGS= -Wall -Wextra -Wshadow -Wformat=2 -fsanitize=address,undefined -Iinclude -Ilib/clay

# --- Directory Structure ---
BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)/bin
OBJ_DIR   = $(BUILD_DIR)/obj
SRC_DIR   = src
TEST_DIR  = tests
RES_DIR   = resources

# --- Core Emulator ---
CORE_SRC = $(SRC_DIR)/core/runner.c $(SRC_DIR)/core/cpu.c $(SRC_DIR)/core/memory.c \
           $(SRC_DIR)/core/instructions.c $(SRC_DIR)/core/io/display.c $(SRC_DIR)/core/io/keyboard.c
CORE_BIN = $(BIN_DIR)/idn16

# --- Disassembler ---
DASM_SRC = $(SRC_DIR)/tools/disassembler/disassembler.c
DASM_INC = $(SRC_DIR)/tools/disassembler/dasm.c
DASM_BIN = $(BIN_DIR)/dasm

# --- Assembler (Flex/Bison) ---
LEX	= flex
YACC = bison
ASM_DIR     = $(SRC_DIR)/tools/assembler
PARSER      = $(ASM_DIR)/parser.y
PARSER_TABH = $(ASM_DIR)/parser.tab.h
PARSER_TABC = $(ASM_DIR)/parser.tab.c
LEXER       = $(ASM_DIR)/lexer.l
LEXYY       = $(ASM_DIR)/lex.yy.c
ASM_BIN     = $(BIN_DIR)/asm

ASM_SRC = $(ASM_DIR)/asm_main.c $(ASM_DIR)/codegen.c $(ASM_DIR)/symbol_table.c

# --- Unit Tests ---
TEST_MEM_SRC    = $(TEST_DIR)/core/test_memory.c
TEST_CPU_SRC    = $(TEST_DIR)/core/test_cpu.c
TEST_SYMTAB_SRC = $(TEST_DIR)/tools/test_symbol_table.c
TEST_DASM_SRC   = $(TEST_DIR)/tools/test_disassembler.c
TEST_CODEGEN_SRC= $(TEST_DIR)/tools/test_codegen.c
UNITY_SRC       = $(TEST_DIR)/unity/unity.c
TEST_MEM_BIN    = $(BIN_DIR)/test_memory
TEST_CPU_BIN    = $(BIN_DIR)/test_cpu
TEST_SYMTAB_BIN = $(BIN_DIR)/test_symbol_table
TEST_DASM_BIN   = $(BIN_DIR)/test_disassembler
TEST_CODEGEN_BIN= $(BIN_DIR)/test_codegen

.PHONY: all core asmblr disasmblr test run_test_memory run_test_cpu run_test_symbol_table run_test_disassembler run_test_codegen clean dirs

all: dirs core test asmblr

# --- Create directory structure ---
dirs:
	@mkdir -p $(BIN_DIR) $(OBJ_DIR) $(TEST_DIR)/core $(TEST_DIR)/tools $(TEST_DIR)/unity

# --- Core Emulator ---
core: $(CORE_BIN)

$(CORE_BIN): $(CORE_SRC) $(DASM_INC) | dirs
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2 -lSDL2_ttf

# --- Assembler ---
asmblr: $(ASM_BIN)

$(PARSER_TABC) $(PARSER_TABH): $(PARSER)
	$(YACC) -Wcex -d -o $(PARSER_TABC) $(PARSER)

$(LEXYY): $(LEXER) $(PARSER_TABH)
	$(LEX) -o $(LEXYY) $(LEXER)

$(ASM_BIN): $(LEXYY) $(PARSER_TABC) $(PARSER_TABH) $(ASM_SRC) | dirs
	$(CC) $(CFLAGS) $(LFLAGS) -o $@ $(LEXYY) $(PARSER_TABC) $(ASM_SRC) -lfl

# --- Disassembler ---
disasmblr: $(DASM_BIN)

$(DASM_BIN): $(DASM_SRC) $(DASM_INC) | dirs
	$(CC) $(CFLAGS) -o $@ $^

# --- Object file rule for tools (optional, for future use) ---
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | dirs
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# --- Unit Tests ---
test: run_test_memory run_test_cpu run_test_symbol_table run_test_disassembler run_test_codegen

run_test_memory: $(TEST_MEM_BIN)
	cd $(BIN_DIR) && ./$(notdir $(TEST_MEM_BIN))

run_test_cpu: $(TEST_CPU_BIN)
	cd $(BIN_DIR) && ./$(notdir $(TEST_CPU_BIN))

run_test_symbol_table: $(TEST_SYMTAB_BIN)
	cd $(BIN_DIR) && ./$(notdir $(TEST_SYMTAB_BIN))

run_test_disassembler: $(TEST_DASM_BIN)
	cd $(BIN_DIR) && ./$(notdir $(TEST_DASM_BIN))

run_test_codegen: $(TEST_CODEGEN_BIN)
	cd $(BIN_DIR) && ./$(notdir $(TEST_CODEGEN_BIN))

$(TEST_MEM_BIN): $(TEST_MEM_SRC) $(UNITY_SRC) $(SRC_DIR)/core/memory.c $(SRC_DIR)/core/instructions.c | dirs
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(TEST_CPU_BIN): $(TEST_CPU_SRC) $(UNITY_SRC) $(SRC_DIR)/core/cpu.c $(SRC_DIR)/core/memory.c $(SRC_DIR)/core/instructions.c $(DASM_INC) | dirs
	$(CC) $(CFLAGS) -o $@ $^ -lSDL2

$(TEST_SYMTAB_BIN): $(TEST_SYMTAB_SRC) $(UNITY_SRC) $(ASM_DIR)/symbol_table.c | dirs
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_DASM_BIN): $(TEST_DASM_SRC) $(UNITY_SRC) $(SRC_DIR)/tools/disassembler/dasm.c | dirs
	$(CC) $(CFLAGS) -o $@ $^

$(TEST_CODEGEN_BIN): $(TEST_CODEGEN_SRC) $(UNITY_SRC) $(ASM_DIR)/codegen.c $(ASM_DIR)/symbol_table.c | dirs
	$(CC) $(CFLAGS) -o $@ $^

# --- Clean ---
clean:
	rm -rf $(BUILD_DIR)
	rm -f $(LEXYY) $(PARSER_TABC) $(PARSER_TABH)
	rm -f *.o *.bin

# --- Utilities ---
install-deps:
	sudo apt update
	sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev flex bison git

dev: clean all

# --- Quick access aliases ---
run: $(CORE_BIN)
	cd $(BIN_DIR) && ./idn16

assemble: $(ASM_BIN)
	@echo "Usage: make assemble PROG=program.asm OUT=output.bin"
	@echo "Example: make assemble PROG=examples/hello.asm OUT=hello.bin"
ifdef PROG
ifdef OUT
	cd $(BIN_DIR) && ./asm ../$(PROG) ../$(RES_DIR)/roms/$(OUT)
else
	@echo "Error: OUT variable not set"
endif
else
	@echo "Error: PROG variable not set"
endif

disassemble: $(DASM_BIN)
	@echo "Usage: make disassemble FILE=binary.bin"
ifdef FILE
	cd $(BIN_DIR) && ./dasm ../$(FILE)
else
	@echo "Error: FILE variable not set"
endif