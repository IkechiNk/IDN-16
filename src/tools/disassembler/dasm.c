#include <stdio.h>
#include <stdlib.h>
#include "idn16/dasm.h"

static int pc = 0;

// Helper to determine system endianess (important for proper code ordering)
static int is_little_endian(void) {
    uint16_t value = 0x0001;
    return *((uint8_t*)&value);  // Returns 1 if little endian, 0 if big endian
}

uint16_t read_word(uint8_t *buffer, long i) {
    uint16_t word;
    if (is_little_endian()) {
        word = (buffer[i + 1] << 8) | buffer[i];
    } else {
        word = (buffer[i] << 8) | buffer[i + 1];
    }
    return word;
}

// Helper to sign-extend a value of `bits` width.
static int32_t sign_extend(uint32_t val, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (int32_t)((val ^ m) - m);
}

char* disassemble_word(uint16_t word) {
    uint8_t opcode = (word >> 11) & 0x1F;
    static char result[64];  // Static buffer to avoid memory leaks
    // REG-format
    if (opcode <= 0b00111) {
        uint8_t rd  = (word >> 8) & 0x07;
        uint8_t rs1 = (word >> 5) & 0x07;
        uint8_t rs2 = (word >> 2) & 0x07;
        uint8_t func=  word       & 0x03;

        switch (opcode) {
        case 0b00000: sprintf(result, "ADD  r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00001: sprintf(result, "SUB  r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00010: sprintf(result, "AND  r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00011: sprintf(result, "OR   r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00100: sprintf(result, "XOR  r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00101: sprintf(result, "SHL  r%d, r%d, r%d\n", rd, rs1, rs2); 
        pc += 2;
        return result;
        case 0b00110:
            if (func == 0) sprintf(result, "SHR  r%d, r%d, r%d\n", rd, rs1, rs2);
            else          sprintf(result, "SRA  r%d, r%d, r%d\n", rd, rs1, rs2);
            
            pc += 2;
            return result;
        case 0b00111:
            if (func == 0) sprintf(result, "MOV  r%d, r%d\n", rd, rs1);
            else if (func == 1) sprintf(result, "CMP  r%d, r%d\n", rd, rs1);
            else                sprintf(result, "NOT  r%d, r%d\n", rd, rs1);
            
            pc += 2;
            return result;
        }
    }

    // IMM-format
    if (opcode >= 0b01000 && opcode <= 0b01111) {
        uint8_t rd  = (word >> 8) & 0x07;
        uint8_t rs1 = (word >> 5) & 0x07;
        int32_t imm;

        switch (opcode) {
        case 0b01000: // LDI: 8-bit immediate
            imm = word & 0xFF;
            sprintf(result, "LDI  r%d, 0x%02X\n", rd, imm);
            
            pc += 2;
            return result;
        case 0b01100: // LUI: upper 8 bits
            imm = word & 0xFF;
            sprintf(result, "LUI  r%d, 0x%02X\n", rd, imm);
            
            pc += 2;
            return result;
        default:
            // 5-bit immediate
            imm = sign_extend(word & 0x1F, 5);
            switch (opcode) {
            case 0b01001: sprintf(result, "LDW  r%d, [r%d+%d]\n", rd, rs1, imm); 
            pc += 2;
            return result;
            case 0b01010: sprintf(result, "STW  r%d, [r%d+%d]\n", rd, rs1, imm); 
            pc += 2;
            return result;
            case 0b01011: sprintf(result, "ADDI r%d, r%d, %d\n", rd, rs1, imm); 
            pc += 2;
            return result;
            case 0b01101: sprintf(result, "ANDI r%d, r%d, %d\n", rd, rs1, imm); 
            pc += 2;
            return result;
            case 0b01110: sprintf(result, "ORI  r%d, r%d, %d\n", rd, rs1, imm); 
            pc += 2;
            return result;
            case 0b01111: sprintf(result, "XORI r%d, r%d, %d\n", rd, rs1, imm); 
            pc += 2;
            return result;
            }
        }
    }

    // JB-format
    if (opcode >= 0b10000 && opcode <= 0b10110) {
        switch (opcode) {
        case 0b10000: {
            int32_t offset = sign_extend(word & 0x7FF, 11);
            sprintf(result, "JMP  %d\n", offset); 
            pc += 2;
            return result;
        }
        case 0b10001: {
            int32_t offset = sign_extend(word & 0x7FF, 11);
            sprintf(result, "JEQ  %d\n", offset); 
            pc += 2;
            return result;
        }
        case 0b10010: {
            int32_t offset = sign_extend(word & 0x7FF, 11);
            sprintf(result, "JNE  %d\n", offset); 
            pc += 2;
            return result;
        }
        case 0b10011: {
            int32_t offset = sign_extend(word & 0x7FF, 11);
            sprintf(result, "JGT  %d\n", offset); 
            pc += 2;
            return result;
        }
        case 0b10100: {
            int32_t offset = sign_extend(word & 0x7FF, 11);
            sprintf(result, "JLT  %d\n", offset); 
            pc += 2;
            return result;
        }
        case 0b10101: {
            // JSR now uses register format: rs1 contains the target address
            uint8_t rs1 = (word >> 5) & 0x07;
            sprintf(result, "JSR  r%d\n", rs1); 
            pc += 2;
            return result;
        }
        case 0b10110: sprintf(result, "RET\n"); 
        pc += 2;
        return result;
        }
    }

    // SP-format
    if (opcode >= 0b11000 && opcode <= 0b11111) {
        uint8_t rd  = (word >> 8) & 0x07;
        uint8_t rs1 = (word >> 5) & 0x07;
        uint8_t imm = sign_extend(word & 0x1F, 5);
        switch (opcode) {
        case 0b11000: sprintf(result, "HLT\n"); 
        pc += 2;
        return result;
        case 0b11001: sprintf(result, "NOP\n"); 
        pc += 2;
        return result;
        case 0b11010: sprintf(result, "INC  r%d\n", rd); 
        pc += 2;
        return result;
        case 0b11011: sprintf(result, "DEC  r%d\n", rd); 
        pc += 2;
        return result;
        case 0b11100: sprintf(result, "LDB  r%d, [r%d+%d]\n", rd, rs1, imm); 
        pc += 2;
        return result;
        case 0b11101: sprintf(result, "STB  r%d, [r%d+%d]\n", rd, rs1, imm); 
        pc += 2;
        return result;
        }
    }

    // Fallback
    sprintf(result, ".word 0x%04X\n", word);
    pc += 2;
    return result;
}