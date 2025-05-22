#ifndef CODEGEN_H
#define CODEGEN_H


#include <stdint.h>

// Emit a REG-format (register) instruction
void emit_reg_format(uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t func);

// Emit an IMM-format (immediate) instruction
void emit_imm_format(uint8_t opcode, uint8_t rd, uint8_t rs1, int8_t  imm);

// Emit a JB-format (branch/jump) instruction with a label to be resolved later
void emit_jb_format_label(uint8_t opcode, const char* label_name);

// Emit a JB-format (branch/jump) instruction with an offset/immediate
void emit_jb_format_imm(uint8_t opcode, uint16_t offset);

// Emit an SP-format (special) instruction
void emit_special_format(uint8_t opcode, uint8_t rd, uint8_t misc, uint8_t extra);

// Return the current program-counter (in instruction words)
int get_pc(void);

// After all emits, resolve labels and write out the final binary
void finalize_output(const char* filename);

#endif // CODEGEN_H
