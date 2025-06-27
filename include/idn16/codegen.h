#ifndef CODEGEN_H
#define CODEGEN_H


#include <stdint.h>

// Emit a REG-format (register) instruction
void emit_reg_format(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, uint8_t rs2, uint8_t func);

// Emit an IMM-format (immediate) instruction
void emit_imm_format_with_line(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, int16_t imm, int line_num);

// Emit an IMM-format instruction with identifier to be resolved later
void emit_imm_format_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t rs1, const char* var_name, int line_num);

// Emit a JB-format (branch/jump) instruction with an identifier to be resolved later
void emit_jb_format_identifier(uint16_t pc, uint8_t opcode, const char* label_name);

// Emit a JB-format (branch/jump) instruction with an offset/immediate
void emit_jb_format_imm(uint16_t pc, uint8_t opcode, uint16_t offset);

// Emit an SP-format (special) instruction
void emit_special_format(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, uint8_t extra);

// Emit an SP-format instruction with identifier to be resolved later
void emit_special_format_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, const char* var_name, int line_num);

// Emit an SP-format instruction with negative identifier to be resolved later
void emit_special_format_neg_identifier(uint16_t pc, uint8_t opcode, uint8_t rd, uint8_t misc, const char* var_name, int line_num);

// Emit LOAD16 macro with identifier
void emit_load16_identifier(uint16_t pc, uint8_t rd, const char* var_name, int line_num);

// Emit PUSH pseudo-instruction
void emit_push_pseudo(uint16_t pc, uint8_t reg);

// Emit POP pseudo-instruction
void emit_pop_pseudo(uint16_t pc, uint8_t reg);


// After all emits, resolve identifiers and write out the final binary
void finalize_output(const char* filename);

#endif // CODEGEN_H
