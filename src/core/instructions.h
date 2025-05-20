#ifndef IDN16_INSTRUCTIONS_H
#define IDN16_INSTRUCTIONS_H

#include "cpu.h"
// REG-format
#define ADD   0
#define SUB   1
#define AND   2
#define OR    3
#define XOR   4
#define SHL   5
#define SHR   6
#define SRA   7
#define MOV   8
#define CMP   9
#define NOT  10

// IMM-format
#define LDI  11
#define LD   12
#define ST   13
#define ADDI 14
#define LUI  15
#define ANDI 16
#define ORI  17
#define XORI 18

// J-format
#define JMP  19
#define JEQ  20
#define JNE  21
#define JGT  22
#define JLT  23
#define JSR  24
#define RET  25

// SP-format
#define HLT  26
#define NOP  27
#define	INC  28
#define	DEC  29
#define	PUSH 30
#define	POP  31

typedef enum { FMT_REG, FMT_IMM, FMT_JB, FMT_SP } FormatType;

typedef struct {
    const char* name;
    uint8_t     opcode;      // 5 bits
    uint8_t     funct;       // lower 2 or 8 bits as needed
    uint8_t     arg_count;
    FormatType  fmt;
} Instruction;

// note: funct is only used by REG- and SP- if needed; IMM ignores it
static Instruction INSTRUCTION_SET[] = {
    // REG-format (3 args except MOV/CMP/NOT use 2 but still REG)
    {"ADD",  0b00000, 0, 3, FMT_REG},
    {"SUB",  0b00001, 0, 3, FMT_REG},
    {"AND",  0b00010, 0, 3, FMT_REG},
    {"OR",   0b00011, 0, 3, FMT_REG},
    {"XOR",  0b00100, 0, 3, FMT_REG},
    {"SHL",  0b00101, 0, 3, FMT_REG},
    {"SHR",  0b00110, 0, 3, FMT_REG},
    {"SRA",  0b00110, 1, 3, FMT_REG},
    {"MOV",  0b00111, 0, 2, FMT_REG},
    {"CMP",  0b00111, 1, 2, FMT_REG},
    {"NOT",  0b00111, 2, 2, FMT_REG},

    // IMM-format (2 or 3 args)
    {"LDI",  0b01000, 0, 2, FMT_IMM},
    {"LD",   0b01001, 0, 3, FMT_IMM},
    {"ST",   0b01010, 0, 3, FMT_IMM},
    {"ADDI", 0b01011, 0, 3, FMT_IMM},
    {"LUI",  0b01100, 0, 2, FMT_IMM},
    {"ANDI", 0b01101, 0, 3, FMT_IMM},
    {"ORI",  0b01110, 0, 3, FMT_IMM},
    {"XORI", 0b01111, 0, 3, FMT_IMM},

    // JB-format (0 or 1 args)
    {"JMP",  0b10000, 0, 1, FMT_JB},
    {"JEQ",  0b10001, 0, 1, FMT_JB},
    {"JNE",  0b10010, 0, 1, FMT_JB},
    {"JGT",  0b10011, 0, 1, FMT_JB},
    {"JLT",  0b10100, 0, 1, FMT_JB},
    {"JSR",  0b10101, 0, 1, FMT_JB},
    {"RET",  0b10110, 0, 0, FMT_JB},

    // SP-format (0 or 1 args)
    {"HLT",  0b11000, 0, 0, FMT_SP},
    {"NOP",  0b11001, 0, 0, FMT_SP},
    {"INC",  0b11010, 0, 1, FMT_SP},
    {"DEC",  0b11011, 0, 1, FMT_SP},
    {"PUSH", 0b11100, 0, 1, FMT_SP},
    {"POP",  0b11101, 0, 1, FMT_SP},
};

static const int INSTR_COUNT = sizeof(INSTRUCTION_SET) / sizeof(Instruction);
void add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void mov(uint16_t rd, uint16_t rs1, Cpu_T *cpu);
void cmp(uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void not(uint16_t rd, uint16_t rs1, Cpu_T *cpu);
void ldi(uint16_t rd, uint16_t imm, Cpu_T *cpu);
void ld(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void st(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void addi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void subi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void andi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void ori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void xori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
void jmp(uint16_t imm, Cpu_T *cpu);
void jeq(uint16_t imm, Cpu_T *cpu);
void jne(uint16_t imm, Cpu_T *cpu);
void jgt(uint16_t imm, Cpu_T *cpu);
void jlt(uint16_t imm, Cpu_T *cpu);
void jsr(uint16_t imm, Cpu_T *cpu);
void ret(Cpu_T *cpu);

void hlt(Cpu_T *cpu);
void nop(Cpu_T *cpu);
void inc(uint16_t rd, Cpu_T *cpu);
void dec(uint16_t rd, Cpu_T *cpu);
void push(uint16_t rd, Cpu_T *cpu);
void pop(uint16_t rd, Cpu_T *cpu);
void lui(uint16_t rd, uint16_t imm, Cpu_T *cpu);

#endif // IDN16_INSTRUCTIONS_H