#include "include/cpu.h"
// REG-format
#define ADD 0
#define SUB 1
#define AND 2
#define OR  3
#define XOR 4
#define SHL 5
#define SHR 6
#define SRA 7
#define MOV 8
#define CMP 9
#define NOT 10

// IMM-format
#define LDI 11
#define LD  12
#define ST  13
#define ADDI 14
#define SUBI 15
#define ANDI 16
#define ORI 17
#define XORI 18

// J-format
#define JMP 19
#define JEQ 20
#define JNE 21
#define JGT 22
#define JLT 23
#define JSR 24
#define RET 25

// SP-format
#define HLT 26
#define NOP 27
#define	INC 28
#define	DEC 29
#define	PUSH 30
#define	POP 31

void add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
void mov(uint16_t rd, uint16_t rs1, Cpu_T *cpu);
void cmp(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
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
void inc(uint16_t rs1, Cpu_T *cpu);
void dec(uint16_t rs1, Cpu_T *cpu);
void push(Cpu_T *cpu);
void pop(Cpu_T *cpu);