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

uint16_t add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t mov(uint16_t rd, uint16_t rs1, Cpu_T *cpu);
uint16_t cmp(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu);
uint16_t not(uint16_t rd, uint16_t rs1, Cpu_T *cpu);

uint16_t ldi(uint16_t rd, uint16_t imm, Cpu_T *cpu);
uint16_t ld(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t st(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t addi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t subi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t andi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t ori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);
uint16_t xori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu);

uint16_t jmp(uint16_t imm, Cpu_T *cpu);
uint16_t jeq(uint16_t imm, Cpu_T *cpu);
uint16_t jne(uint16_t imm, Cpu_T *cpu);
uint16_t jgt(uint16_t imm, Cpu_T *cpu);
uint16_t jlt(uint16_t imm, Cpu_T *cpu);
uint16_t jsr(uint16_t imm, Cpu_T *cpu);
uint16_t ret(Cpu_T *cpu);

void hlt(Cpu_T *cpu);
void nop(Cpu_T *cpu);
uint16_t inc(uint16_t rs1, Cpu_T *cpu);
uint16_t dec(uint16_t rs1, Cpu_T *cpu);
void push(Cpu_T *cpu);
uint16_t pop(Cpu_T *cpu);