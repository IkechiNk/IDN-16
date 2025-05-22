#include "codegen.h"
#include "symbol_table.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define MAX_INSNS 8192

static uint16_t output[MAX_INSNS];
static int pc = 0;

typedef struct {
    int pc_index;
    char* name;
    uint8_t opcode;
} REFERENCE;

static REFERENCE refs[MAX_LABEL*2];
static int label_cnt = 0;

int get_pc(void) {
    return pc;
}

void emit_reg_format(uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t func) {
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
    pc += 2;
}

void emit_imm_format(uint8_t opcode, uint8_t rd, uint8_t rs1, int8_t imm) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t imm5 = (imm) & 0x1FF;
    uint16_t insn = (opcode << 11)
                  | (rd     << 8)
                  | (rs1    << 5)
                  | imm5;
    output[pc/2] = insn;
    pc += 2;
}

void emit_jb_format_label(uint8_t opcode, const char* label_name) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    } else if (label_cnt >= MAX_LABEL) {
        fprintf(stderr, "Error: Too many labels provided.\n");
        exit(1); 
    }
    refs[label_cnt].pc_index = pc;
    refs[label_cnt].opcode = opcode;
    refs[label_cnt].name = strdup(label_name);
    output[pc/2] = 0;  
    pc += 2;
    label_cnt++;
}

void emit_jb_format_imm(uint8_t opcode, uint16_t offset) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t insn = ((uint16_t) opcode << 11) | offset;
    output[pc/2] = insn;
    pc += 2;
}

void emit_special_format(uint8_t opcode, uint8_t rd, uint8_t misc, uint8_t extra) {
    if (pc >= MAX_INSNS) {
        fprintf(stderr, "Error: Max instruction count reached. (Defined in tools/assembler/codegen.c)\n");
        exit(1);
    }
    uint16_t insn;
    if (opcode >= 0b11110) {
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
    pc += 2;
}

void finalize_output(const char* filename) {
    // Resolve all pending name references
    sort_labels();
    for (int i = 0; i < label_cnt; i++) {
        int jmp_pc  = refs[i].pc_index;
        int label_addr = get_label(refs[i].name);
        int offset = label_addr - jmp_pc;
        
        uint16_t u_offset = (uint16_t)offset & 0b11111111111; 
        uint16_t word = ((uint16_t)refs[i].opcode << 11)
                      | (u_offset);
        output[jmp_pc/2] = word;
        free(refs[i].name);
    }

    // Write binary file
    FILE* f = fopen(filename, "wb");
    if (!f) {
        perror("fopen output");
        exit(1);
    }
    if (fwrite(output, sizeof(uint16_t), pc/2, f) != (size_t)pc/2) {
        perror("fwrite");
        exit(1);
    }
    fclose(f);
}
