#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_BYTES 65536  // Maximum file size (64KB)

// Helper to sign-extend a value of `bits` width.
static int32_t sign_extend(uint32_t val, int bits) {
    uint32_t m = 1u << (bits - 1);
    return (int32_t)((val ^ m) - m);
}

void disassemble_word(uint16_t word) {
    uint8_t opcode = (word >> 11) & 0x1F;

    // REG-format
    if (opcode <= 0b00111) {
        uint8_t rd  = (word >> 8) & 0x07;
        uint8_t rs1 = (word >> 5) & 0x07;
        uint8_t rs2 = (word >> 2) & 0x07;
        uint8_t func=  word       & 0x03;

        switch (opcode) {
        case 0b00000: printf("ADD  r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00001: printf("SUB  r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00010: printf("AND  r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00011: printf("OR   r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00100: printf("XOR  r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00101: printf("SHL  r%d, r%d, r%d\n", rd, rs1, rs2); return;
        case 0b00110:
            if (func == 0) printf("SHR  r%d, r%d, r%d\n", rd, rs1, rs2);
            else          printf("SRA  r%d, r%d, r%d\n", rd, rs1, rs2);
            return;
        case 0b00111:
            if (func == 0) printf("MOV  r%d, r%d\n", rd, rs1);
            else if (func == 1) printf("CMP  r%d, r%d\n", rd, rs2);
            else                printf("NOT  r%d, r%d\n", rd, rs1);
            return;
        }
    }

    // IMM-format
    if (opcode >= 0b01000 && opcode <= 0b01111) {
        uint8_t rd  = (word >> 8) & 0x07;
        uint8_t rs1 = (word >> 5) & 0x07;
        int32_t imm;

        switch (opcode) {
        case 0b01000: // LDI: 8-bit immediate
            imm = sign_extend(word & 0xFF, 8);
            printf("LDI  r%d, %d\n", rd, imm);
            return;
        case 0b01100: // LUI: upper 8 bits
            imm = (word & 0xFF) << 8;
            printf("LUI  r%d, 0x%X\n", rd, imm);
            return;
        default:
            // 5-bit immediate
            imm = sign_extend(word & 0x1F, 5);
            switch (opcode) {
            case 0b01001: printf("LD   r%d, [r%d+%d]\n", rd, rs1, imm); return;
            case 0b01010: printf("ST   r%d, [r%d+%d]\n", rd, rs1, imm); return;
            case 0b01011: printf("ADDI r%d, r%d, %d\n", rd, rs1, imm); return;
            case 0b01101: printf("ANDI r%d, r%d, %d\n", rd, rs1, imm); return;
            case 0b01110: printf("ORI  r%d, r%d, %d\n", rd, rs1, imm); return;
            case 0b01111: printf("XORI r%d, r%d, %d\n", rd, rs1, imm); return;
            }
        }
    }

    // JB-format
    if (opcode >= 0b10000 && opcode <= 0b10110) {
        int32_t offset = sign_extend(word & 0x7FF, 11);
        switch (opcode) {
        case 0b10000: printf("JMP  %d\n", offset); return;
        case 0b10001: printf("JEQ  %d\n", offset); return;
        case 0b10010: printf("JNE  %d\n", offset); return;
        case 0b10011: printf("JGT  %d\n", offset); return;
        case 0b10100: printf("JLT  %d\n", offset); return;
        case 0b10101: printf("JSR  %d\n", offset); return;
        case 0b10110: printf("RET\n"); return;
        }
    }

    // SP-format
    if (opcode >= 0b11000 && opcode <= 0b11101) {
        uint8_t rd = (word >> 8) & 0x07;
        switch (opcode) {
        case 0b11000: printf("HLT\n"); return;
        case 0b11001: printf("NOP\n"); return;
        case 0b11010: printf("INC  r%d\n", rd); return;
        case 0b11011: printf("DEC  r%d\n", rd); return;
        case 0b11100: printf("PUSH r%d\n", rd); return;
        case 0b11101: printf("POP  r%d\n", rd); return;
        }
    }

    // Fallback
    printf(".word 0x%04X\n", word);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input.bin>\n", argv[0]);
        return 1;
    }

    // Open binary file
    FILE *file = fopen(argv[1], "rb");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    // Determine file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    rewind(file);

    if (file_size > MAX_BYTES) {
        fprintf(stderr, "File too large: %ld bytes (max: %d bytes)\n", file_size, MAX_BYTES);
        fclose(file);
        return 1;
    }

    // Read file into buffer
    uint8_t *buffer = malloc(file_size);
    if (!buffer) {
        perror("Memory allocation failed");
        fclose(file);
        return 1;
    }

    if (fread(buffer, 1, file_size, file) != (size_t)file_size) {
        perror("File read error");
        free(buffer);
        fclose(file);
        return 1;
    }

    fclose(file);


    // Print header
    printf("Address: Instruction\n");
    printf("------------------------\n");

    // Disassemble each 16-bit word
    for (long i = 0; i < file_size; i += 2) {
        if (i + 1 < file_size) {
            uint16_t word = (buffer[i] << 8) | buffer[i + 1];
            disassemble_word(word);
        } else {
            printf("Incomplete word found at %d: %02X\n", (int)i, buffer[i]);
        }
    }

    free(buffer);
    return 0;
}
