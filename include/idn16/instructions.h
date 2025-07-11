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
#define LDW  12
#define STW  13
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
#define LDB  30
#define STB  31

void add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu);
void mov(uint16_t rd, uint16_t rs1, Cpu_t *cpu);
void cmp(uint16_t rd, uint16_t rs1, Cpu_t *cpu);
void not(uint16_t rd, uint16_t rs1, Cpu_t *cpu);
void ldi(uint16_t rd, uint16_t imm, Cpu_t *cpu);
void ldw(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void stw(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void addi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void andi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void ori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void xori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu);
void jmp(uint16_t imm, Cpu_t *cpu);
void jeq(uint16_t imm, Cpu_t *cpu);
void jne(uint16_t imm, Cpu_t *cpu);
void jgt(uint16_t imm, Cpu_t *cpu);
void jlt(uint16_t imm, Cpu_t *cpu);
void jsr(uint16_t rd, Cpu_t *cpu);
void ret(Cpu_t *cpu);

void hlt(Cpu_t *cpu);
void nop(Cpu_t *cpu);
void inc(uint16_t rd, Cpu_t *cpu);
void dec(uint16_t rd, Cpu_t *cpu);
void lui(uint16_t rd, uint16_t imm, Cpu_t *cpu);
void stb(uint16_t rd, uint16_t rs1, uint8_t imm, Cpu_t *cpu);
void ldb(uint16_t rd, uint16_t rs1, uint8_t imm, Cpu_t *cpu);

#endif // IDN16_INSTRUCTIONS_H