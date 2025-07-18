#include "idn16/codegen.h"
#include "idn16/symbol_table.h"
#include "idn16/instructions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INSNS 16384

static uint16_t output[MAX_INSNS];

typedef struct {
    int pc_index;
    char* name;
    uint8_t opcode;
    uint8_t rd;
    uint8_t rs1;
    uint8_t misc;
    uint8_t format_type; // 0=JB, 1=IMM, 2=SP, 3=LOAD16
    int line_num;
} REFERENCE;

static REFERENCE refs[MAX_SYMBOL*2];
static int symbol_cnt = 0;
static int isnt_cnt = 0;

void emit_reg_format(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t func) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t insn = (opcode << 11)
                  | (rd     << 8)
                  | (rs1    << 5)
                  | (rs2    << 2)
                  | (func   & 0x3);
    output[pc/2] = insn;
    isnt_cnt = (pc / 2) + 1;
}

void emit_imm_format_with_line(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, int16_t imm, int line_num) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    
    // Size validation for immediate operands using lexer-defined bounds
    if (opcode == 0b01000 && (imm < -128 || imm > 0b11111111)) {
        if (line_num > 0) {
            fprintf(stderr, "Error on line %d: LDI immediate value %d out of range [-128, %d]\n", line_num, imm, 0b11111111);
        } else {
            fprintf(stderr, "Error: LDI immediate value %d out of range [-128, %d]\n", imm, 0b11111111);
        }
        exit(1);
    }
    if (opcode == 0b01100 && (imm < 0 || imm > 0b11111111)) {
        if (line_num > 0) {
            fprintf(stderr, "Error on line %d: LUI immediate value %d out of range [0, %d]\n", line_num, imm, 0b11111111);
        } else {
            fprintf(stderr, "Error: LUI immediate value %d out of range [0, %d]\n", imm, 0b11111111);
        }
        exit(1);
    }
    if (opcode != 0b01000 && opcode != 0b01100 && (imm < -16 || imm > 0b11111)) {
        if (line_num > 0) {
            fprintf(stderr, "Error on line %d: Immediate value %d out of range [-16, %d]\n", line_num, imm, 0b11111);
        } else {
            fprintf(stderr, "Error: Immediate value %d out of range [-16, %d]\n", imm, 0b11111);
        }
        exit(1);
    }
    if (opcode == 0b01100 || opcode == 0b01000) {
        // For LUI and LDI, we use 8-bit immediate
        imm = (imm) & 0xFF;
    } else {
        // For other instructions, we use 5-bit immediate
        imm = (imm) & 0x1F;
    }
    uint16_t insn = (opcode << 11)
                  | (rd     << 8)
                  | (rs1    << 5)
                  | imm;
    output[pc/2] = insn;
    isnt_cnt = (pc / 2) + 1;
}

void emit_jb_format_identifier(uint16_t pc, uint8_t opcode, const char* label_name) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (symbol_cnt >= MAX_SYMBOL) {
        fprintf(stderr, "Error: Too many symbols provided.\n");
        exit(1); 
    }
    refs[symbol_cnt].pc_index = pc;
    refs[symbol_cnt].opcode = opcode;
    refs[symbol_cnt].name = strdup(label_name);
    refs[symbol_cnt].format_type = 0; // JB format
    output[pc/2] = 0;  
    symbol_cnt++;
    isnt_cnt = (pc / 2) + 1;
}

void emit_jb_format_imm(uint16_t pc, uint8_t opcode, uint16_t offset) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t insn = ((uint16_t) opcode << 11) | offset;
    output[pc/2] = insn;
    isnt_cnt = (pc / 2) + 1;
}

void emit_special_format(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, uint8_t extra) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t insn;
    if (opcode >= 0b11100) {
        uint16_t imm5 = (extra) & 0x1FF;
        insn = (opcode << 11)
             | (rd     << 8)
             | (misc << 5)
             | (imm5);
    } else {
        insn = (opcode << 11)
             | (rd << 8)
             | (misc & 0xFF);
    }
    output[pc/2] = insn;
    isnt_cnt = (pc / 2) + 1;
}

void emit_imm_format_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, const char* var_name, int line_num) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (symbol_cnt >= MAX_SYMBOL) {
        fprintf(stderr, "Error: Too many symbols provided.\n");
        exit(1); 
    }
    refs[symbol_cnt].pc_index = pc;
    refs[symbol_cnt].opcode = opcode;
    refs[symbol_cnt].rd = rd;
    refs[symbol_cnt].rs1 = rs1;
    refs[symbol_cnt].format_type = 1; // IMM format
    refs[symbol_cnt].name = strdup(var_name);
    refs[symbol_cnt].line_num = line_num;
    output[pc/2] = 0;
    symbol_cnt++;
    isnt_cnt = (pc / 2) + 1;
}

void emit_special_format_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, const char* var_name, int line_num) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (symbol_cnt >= MAX_SYMBOL) {
        fprintf(stderr, "Error: Too many symbols provided.\n");
        exit(1); 
    }
    refs[symbol_cnt].pc_index = pc;
    refs[symbol_cnt].opcode = opcode;
    refs[symbol_cnt].rd = rd;
    refs[symbol_cnt].misc = misc;
    refs[symbol_cnt].format_type = 2; // SP format
    refs[symbol_cnt].name = strdup(var_name);
    refs[symbol_cnt].line_num = line_num;
    output[pc/2] = 0;
    symbol_cnt++;
    isnt_cnt = (pc / 2) + 1;
}

void emit_special_format_neg_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, const char* var_name, int line_num) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (symbol_cnt >= MAX_SYMBOL) {
        fprintf(stderr, "Error: Too many symbols provided.\n");
        exit(1); 
    }
    refs[symbol_cnt].pc_index = pc;
    refs[symbol_cnt].opcode = opcode;
    refs[symbol_cnt].rd = rd;
    refs[symbol_cnt].misc = misc;
    refs[symbol_cnt].format_type = 4; // SP format with negative identifier
    refs[symbol_cnt].name = strdup(var_name);
    refs[symbol_cnt].line_num = line_num;
    output[pc/2] = 0;
    symbol_cnt++;
    isnt_cnt = (pc / 2) + 1;
}

void emit_load16_identifier(uint16_t pc, uint8_t rd, const char* var_name, int line_num) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (symbol_cnt >= MAX_SYMBOL) {
        fprintf(stderr, "Error: Too many symbols provided.\n");
        exit(1); 
    }
    refs[symbol_cnt].pc_index = pc;
    refs[symbol_cnt].rd = rd;
    refs[symbol_cnt].format_type = 3; // LOAD16 macro
    refs[symbol_cnt].name = strdup(var_name);
    refs[symbol_cnt].line_num = line_num;
    output[pc/2] = 0;
    output[(pc+2)/2] = 0;
    symbol_cnt++;
    isnt_cnt = ((pc + 2) / 2) + 1;
}


void finalize_output(const char* filename) {
    // Resolve all pending name references
    sort_symbols();
    for (int i = 0; i < symbol_cnt; i++) {
        int pc_addr = refs[i].pc_index;
        int type;
        int value = get_symbol(refs[i].name, &type);
        
        if (refs[i].format_type == 0) {
            // JB format - handle jumps and branches
            if (type == 0) {
                int offset = value - pc_addr;    
                uint16_t u_offset = (uint16_t)offset & 0b11111111111; 
                uint16_t word = ((uint16_t)refs[i].opcode << 11) | (u_offset);
                output[pc_addr/2] = word;
            }
        } else if (refs[i].format_type == 1) {
            // IMM format - handle immediate values with variables
            if (type == 1) { // Variable/constant
                // Size validation for immediate operands using lexer-defined bounds
                if (refs[i].opcode == 0b01000 && (value < -128 || value > 0b11111111)) {
                    fprintf(stderr, "Error on line %d: LDI immediate value %d out of range [-128, %d] for variable %s\n", refs[i].line_num, value, 0b11111111, refs[i].name);
                    exit(1);
                }
                if (refs[i].opcode != 0b01000 && (value < -16 || value > 0b11111)) {
                    fprintf(stderr, "Error on line %d: Immediate value %d out of range [-16, %d] for variable %s\n", refs[i].line_num, value, 0b11111, refs[i].name);
                    exit(1);
                }
                
                uint16_t imm5 = (value) & 0x1FF;
                uint16_t insn = (refs[i].opcode << 11)
                              | (refs[i].rd << 8)
                              | (refs[i].rs1 << 5)
                              | imm5;
                output[pc_addr/2] = insn;
            }
        } else if (refs[i].format_type == 2) {
            // SP format - handle special format with identifiers
            if (type == 1) { // Variable/constant
                uint16_t insn;
                if (refs[i].opcode >= 0b11100) {
                    uint16_t imm5 = (value) & 0x1FF;
                    insn = (refs[i].opcode << 11)
                         | (refs[i].rd << 8)
                         | (refs[i].misc << 5)
                         | (imm5);
                } else {
                    insn = (refs[i].opcode << 11)
                         | (refs[i].rd << 8)
                         | (value & 0xFF);
                }
                output[pc_addr/2] = insn;
            }
        } else if (refs[i].format_type == 4) {
            // SP format with negative identifier - handle special format with negative identifiers
            if (type == 1) { // Variable/constant
                int neg_value = -value; // Negate the identifier value
                uint16_t insn;
                if (refs[i].opcode >= 0b11100) {
                    uint16_t imm5 = (neg_value) & 0x1FF;
                    insn = (refs[i].opcode << 11)
                         | (refs[i].rd << 8)
                         | (refs[i].misc << 5)
                         | (imm5);
                } else {
                    insn = (refs[i].opcode << 11)
                         | (refs[i].rd << 8)
                         | (neg_value & 0xFF);
                }
                output[pc_addr/2] = insn;
            }
        } else if (refs[i].format_type == 3) {
            // LOAD16 macro - loads a 16-bit value using LDI and LUI (per README spec)
            if (type == 1 || type == 0) { // Variable/constant or label address
                uint8_t high_byte = (value >> 8) & 0xFF;
                uint8_t low_byte = value & 0xFF;
                
                // First instruction: LDI rd, imm16[7:0] (low byte)
                uint16_t ldi_insn = (0b01000 << 11) | (refs[i].rd << 8) | low_byte;
                output[pc_addr/2] = ldi_insn;
                
                // Second instruction: LUI rd, imm16[15:8] (high byte)
                if (pc_addr + 2 < MAX_INSNS * 2) {
                    uint16_t lui_insn = (0b01100 << 11) | (refs[i].rd << 8) | high_byte;
                    output[(pc_addr + 2)/2] = lui_insn;
                    // Only update isnt_cnt if this position is beyond the current count
                    int new_cnt = ((pc_addr + 2) / 2) + 1;
                    if (new_cnt > isnt_cnt) {
                        isnt_cnt = new_cnt;
                    }
                } else {
                    fprintf(stderr, "Error: LOAD16 macro would exceed instruction memory\n");
                    exit(1);
                }
            }
        }
        
        free(refs[i].name);
    }

    // Write binary file
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("fopen output");
        exit(1);
    }
    if (fwrite(output, sizeof(uint16_t), isnt_cnt, f) != (size_t)isnt_cnt) {
        perror("fwrite");
        exit(1);
    }
    fclose(f);
}

void emit_push_pseudo(uint16_t pc, uint8_t reg) {
    // PUSH reg expands to:
    // 1. ADDI sp, sp, -2  - Decrement stack pointer
    // 2. STW reg, [sp]    - Store register to stack
    
    // First instruction: ADDI sp, sp, -2
    emit_imm_format_with_line(pc, 0b01011, 6, 6, -2, 0);  // ADDI r6(sp), r6(sp), -2
    
    // Second instruction: STW reg, [sp+0]
    emit_imm_format_with_line(pc + 2, 0b01010, reg, 6, 0, 0);  // STW reg, [r6(sp)+0]
}

void emit_pop_pseudo(uint16_t pc, uint8_t reg) {
    // POP reg expands to:
    // 1. LDW reg, [sp]    - Load from stack to register
    // 2. ADDI sp, sp, 2   - Increment stack pointer
    
    // First instruction: LDW reg, [sp+0]
    emit_imm_format_with_line(pc, 0b01001, reg, 6, 0, 0);  // LDW reg, [r6(sp)+0]
    
    // Second instruction: ADDI sp, sp, 2
    emit_imm_format_with_line(pc + 2, 0b01011, 6, 6, 2, 0);  // ADDI r6(sp), r6(sp), 2
}

void reset_codegen(void) {
    // Reset all static variables to their initial state
    symbol_cnt = 0;
    isnt_cnt = 0;
    
    // Clear output array
    for (int i = 0; i < MAX_INSNS; i++) {
        output[i] = 0;
    }
    
    // Clear references array
    for (int i = 0; i < MAX_SYMBOL*2; i++) {
        refs[i].pc_index = 0;
        refs[i].name = NULL;
        refs[i].opcode = 0;
        refs[i].rd = 0;
        refs[i].rs1 = 0;
        refs[i].misc = 0;
        refs[i].format_type = 0;
        refs[i].line_num = 0;
    }
}
