#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "idn16/dasm.h"

#define MAX_BYTES 65536  // Maximum file size (64KB)

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <input.bin> <output.asm>\n", argv[0]);
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

    // Open output file
    FILE *output_file = fopen(argv[2], "w");
    if (!output_file) {
        perror("Error opening output file");
        free(buffer);
        return 1;
    }

    // Write header
    fprintf(output_file, "Instruction\n");
    fprintf(output_file, "------------------------\n");

    // Disassemble each 16-bit word
    for (long i = 0; i < file_size; i += 2) {
        if (i + 1 < file_size) {
            uint16_t word = read_word(buffer, i);
            fprintf(output_file, "%s", disassemble_word(word));
        } else {
            fprintf(output_file, "Incomplete word found at %d: %02X\n", (int)i, buffer[i]);
        }
    }

    fclose(output_file);
    free(buffer);
    return 0;
}
