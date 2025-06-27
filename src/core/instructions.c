#include "idn16/instructions.h"
#include "idn16/memory.h"
#include "idn16/cpu.h"

int16_t sign_extend_5(uint16_t imm) {
    return (int16_t)((imm & 0x10) ? (imm | 0xFFE0) : (imm & 0x1F));
}

int16_t sign_extend_8(uint16_t imm) {
    return (int16_t)((imm & 0x80) ? (imm | 0xFF00) : (imm & 0xFF));
}

int16_t sign_extend_11(uint16_t imm) {
    return (int16_t)((imm & 0x400) ? (imm | 0xF800) : (imm & 0x7FF));
}

void add(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint32_t result32 = (uint32_t)cpu->r[rs1] + (uint32_t)cpu->r[rs2];
    uint16_t result = (uint16_t)result32;
    int16_t srs1 = (int16_t)cpu->r[rs1];
    int16_t srs2 = (int16_t)cpu->r[rs2];
    int16_t sresult = (int16_t)result;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = result32 > 0xFFFF;
    cpu->flags.v = (srs1 > 0 && srs2 > 0 && sresult < 0) || (srs1 < 0 && srs2 < 0 && sresult > 0);
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void sub(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint32_t result = cpu->r[rs1] - cpu->r[rs2];
    int16_t srs1 = (int16_t)cpu->r[rs1];
    int16_t srs2 = (int16_t)cpu->r[rs2];
    int16_t sresult = (int16_t)result;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = (cpu->r[rs1] >= cpu->r[rs2]);
    cpu->flags.v = (srs1 < 0 && srs2 > 0 && sresult > 0) || (srs1 > 0 && srs2 < 0 && sresult < 0);
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void and(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val & rs2_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void or(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val | rs2_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void xor(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t rs2_val = cpu->r[rs2];
    uint16_t result = rs1_val ^ rs2_val;
    
    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void shl(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
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
    cpu->pc += 2;
}
void shr(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
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
    cpu->pc += 2;
}
void sra(uint16_t rd, uint16_t rs1, uint16_t rs2, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
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
    cpu->pc += 2;
}
void mov(uint16_t rd, uint16_t rs1, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    cpu->r[rd] = cpu->r[rs1];
    cpu->pc += 2;
}
void cmp(uint16_t rd, uint16_t rs1, Cpu_t *cpu) {
    uint16_t rs1_val = cpu->r[rd];
    uint16_t rs2_val = cpu->r[rs1];
    int16_t srs1 = (int16_t)rs1_val;
    int16_t srs2 = (int16_t)rs2_val;

    cpu->flags.z = (rs1_val == rs2_val);
    cpu->flags.n = (srs1 < srs2);
    cpu->pc += 2;
}
void not(uint16_t rd, uint16_t rs1, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t result = ~rs1_val;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}

void ldi(uint16_t rd, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    cpu->r[rd] = sign_extend_8(imm);
    cpu->flags.z = (cpu->r[rd] == 0);
    cpu->flags.n = (cpu->r[rd] & 0x8000) != 0;
    cpu->pc += 2;
}
void ldw(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t address = cpu->r[rs1] + sign_extend_5(imm);
    cpu->r[rd] = memory_read_word(cpu->memory, address);
    cpu->flags.z = (cpu->r[rd] == 0);
    cpu->flags.n = (cpu->r[rd] & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->pc += 2;
}
void stw(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    uint16_t address = cpu->r[rs1] + sign_extend_5(imm);
    memory_write_word(cpu->memory, address, cpu->r[rd], false);
    cpu->pc += 2;
}
void addi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint32_t result32 = (uint32_t)cpu->r[rs1] + (uint32_t)((int16_t)sign_extend_5(imm));
    uint16_t result = (uint16_t)result32;
    int16_t srs1 = (int16_t)cpu->r[rs1];
    int16_t simm = (int16_t)sign_extend_5(imm);
    int16_t sresult = (int16_t)result;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = result32 > 0xFFFF;
    cpu->flags.v = (srs1 > 0 && simm > 0 && sresult < 0) || (srs1 < 0 && simm < 0 && sresult > 0);
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void andi(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t result = rs1_val & imm;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void ori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t result = rs1_val | imm;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void xori(uint16_t rd, uint16_t rs1, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t rs1_val = cpu->r[rs1];
    uint16_t result = rs1_val ^ imm;

    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}

void jmp(uint16_t imm, Cpu_t *cpu) {
    cpu->r[7] = cpu->pc + 2; 
    cpu->pc += (int)sign_extend_11(imm);
    cpu->flags.z = (cpu->pc == 0);
}
void jeq(uint16_t imm, Cpu_t *cpu) {
    if (cpu->flags.z) {
        jmp(imm, cpu);
    } else {
        cpu->pc += 2;
    }
}
void jne(uint16_t imm, Cpu_t *cpu) {
    if (!cpu->flags.z) {
        jmp(imm, cpu);
    } else {
        cpu->pc += 2;
    }
}
void jgt(uint16_t imm, Cpu_t *cpu) {
    if (!cpu->flags.z && !cpu->flags.n) {
        jmp(imm, cpu);
    } else {
        cpu->pc += 2;
    }
}
void jlt(uint16_t imm, Cpu_t *cpu) {
    if (cpu->flags.n && !cpu->flags.z) {
        jmp(imm, cpu);
    } else {
        cpu->pc += 2;
    }
}
void jsr(uint16_t rs1, Cpu_t *cpu) {
    // JSR uses register for absolute addressing - rs1 contains the target address
    uint16_t target_address = cpu->r[rs1];
    
    // Check if target is in system call range
    if (target_address >= SYSCALL_BASE && target_address <= SYSCALL_END) {
        handle_system_call(target_address, cpu);
        cpu->pc += 2; // Move to next instruction
        return;
    }
    // Normal subroutine call - save return address and jump to absolute target
    cpu->r[7] = cpu->pc + 2; // Save return address (next instruction)
    cpu->pc = target_address; // Jump to absolute address
}
void ret(Cpu_t *cpu) {
    cpu->pc = cpu->r[7];
}

void hlt(Cpu_t *cpu) {
    cpu->running = false;
    cpu->flags.z = 0;
    cpu->flags.n = 0;
    cpu->flags.c = 0;
    cpu->flags.v = 0;
    cpu->pc += 2;
}
void nop(Cpu_t *cpu) {
    cpu->flags.z = 0;
    cpu->flags.n = 0;
    cpu->flags.c = 0;
    cpu->flags.v = 0;
    mov(0, 0, cpu);
}
void inc(uint16_t rd, Cpu_t *cpu) {
    addi(rd, rd, 1, cpu);
}
void dec(uint16_t rd, Cpu_t *cpu) {
    addi(rd, rd, -1, cpu);
}
void lui(uint16_t rd, uint16_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t upper = (sign_extend_8(imm)) << 8;
    uint16_t result = upper | (cpu->r[rd] & 0x00FF);
    cpu->flags.z = (result == 0);
    cpu->flags.n = (result & 0x8000) != 0;
    cpu->flags.c = 0;
    cpu->flags.v = 0;
    cpu->r[rd] = result;
    cpu->pc += 2;
}
void ldh(uint16_t rd, uint16_t rs1, uint8_t imm, Cpu_t *cpu) {
    if (rd == 0) {
        cpu->r[0] = 0;
        cpu->pc += 2;
        return;   
    }
    uint16_t address = cpu->r[rs1] + sign_extend_5(imm);
    cpu->r[rd] = memory_read_byte(cpu->memory, address);
    cpu->flags.z = (cpu->r[rd] == 0);
    cpu->flags.n = (cpu->r[rd] & 0x8000) != 0;
    cpu->flags.c = 0; 
    cpu->flags.v = 0;
    cpu->pc += 2;
}
void stb(uint16_t rd, uint16_t rs1, uint8_t imm, Cpu_t *cpu) {
    uint16_t address = cpu->r[rs1] + sign_extend_5(imm);
    memory_write_byte(cpu->memory, address, cpu->r[rd], false);
    cpu->pc += 2;
}