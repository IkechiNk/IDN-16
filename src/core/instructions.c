#include "include/instructions.h"

void add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint32_t result32 = (uint32_t)cpu->r[rs1] + (uint32_t)cpu->r[rs2];
    uint16_t result = (uint16_t)result32;
    int16_t srs1 = (int16_t)cpu->r[rs1];
    int16_t srs2 = (int16_t)cpu->r[rs2];
    int16_t sresult = (int16_t)result;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000);
    cpu->flags.c = result32 > 0xFFFF;
    cpu->flags.v = (srs1 > 0 && srs2 > 0 && sresult < 0) || (srs1 < 0 && srs2 < 0 && sresult > 0);
    cpu->r[rd] = result;
}
void sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint32_t result = cpu->r[rs1] - cpu->r[rs2];
    int16_t srs1 = (int16_t)cpu->r[rs1];
    int16_t srs2 = (int16_t)cpu->r[rs2];
    int16_t sresult = (int16_t)result;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = (cpu->r[rs1] >= cpu->r[rs2]);
    cpu->flags.v = (srs1 < 0 && srs2 > 0 && sresult > 0) || (srs1 > 0 && srs2 < 0 && sresult < 0);
    cpu->r[rd] = result;
}
void and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val & rs2_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->r[rd] = result;
}
void or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val | rs2_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->r[rd] = result;
}
void xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val ^ rs2_val;
    
    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->r[rd] = result;
}
void shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val << rs2_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    if (rs2_val > 0 && rs2_val <= 16) {
        cpu->flags.c = (rs1_val >> (16 - rs2_val)) & 1;
    } else {
        cpu->flags.c = 0;
    }
    cpu->r[rd] = result;
}
void shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val >> rs2_val;
    
    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    if (rs2_val > 0 && rs2_val <= 16) {
        cpu->flags.c = (rs1_val >> (rs2_val - 1)) & 1;
    } else {
        cpu->flags.c = 0;
    }
    cpu->r[rd] = result;
}
void sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val >> rs2_val;
    if (rs1_val >> 15) {
        uint16_t digits = 16 - rs2_val; 
        result |= ((uint16_t)0xFFFF << (digits));
    }

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    if (rs2_val > 0 && rs2_val <= 16) {
        cpu->flags.c = (rs1_val >> (16 - rs2_val)) & 1;
    } else {
        cpu->flags.c = 0;
    }
    cpu->r[rd] = result;
}
void mov(uint16_t rd, uint16_t rs1, Cpu_T *cpu) {
    return 0;
}
void cmp(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_T *cpu) {
    return 0;
}
void not(uint16_t rd, uint16_t rs1, Cpu_T *cpu) {
    return 0;
}
void ldi(uint16_t rd, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void ld(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void st(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void addi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void subi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void andi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void ori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void xori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jmp(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jeq(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jne(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jgt(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jlt(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void jsr(uint16_t imm, Cpu_T *cpu) {
    return 0;
}
void ret(Cpu_T *cpu) {
    return 0;
}
void hlt(Cpu_T *cpu) {
    return NULL;
}
void nop(Cpu_T *cpu) {
    return NULL;
}
void inc(uint16_t rs1, Cpu_T *cpu) {
    return 0;
}
void dec(uint16_t rs1, Cpu_T *cpu) {
    return 0;
}
void push(Cpu_T *cpu) {
    return NULL;;
}
void pop(Cpu_T *cpu) {
    return 0;
}