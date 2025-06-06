#include <stdio.h>
#include <stdlib.h>

#include "symbol_table.h"
#include "codegen.h"

// Flex/Bison externs
extern FILE* yyin;
int yyparse(void);


void add_newline_to_file(const char* filename) {
    FILE *f = fopen(filename, "rb+");
    if (!f) {
        fprintf(stderr, "Error: Unable to open the file");
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    
    if (size == 0) {
        fclose(f);
        exit(1);  // Empty file, consider it as ending with newline
    }
    
    // Check last character
    fseek(f, -1, SEEK_END);
    int last_char = fgetc(f);
    fseek(f, 0, SEEK_END);
    
    if (last_char == '\n') {return;}
    
    // Add newline to end of file
    if (fputc('\n', f) == EOF) {
        fprintf(stderr, "Error: Add a newline to the end of file %s\n", filename);
        fclose(f);
        exit(1);
    }
    printf("Added newline to end of file '%s'\n", filename);
    fclose(f);
    return;
}


int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.asm output.bin\n", argv[0]);
        return EXIT_FAILURE;
    }
    add_newline_to_file(argv[1]);
    yyin = fopen(argv[1], "rw");
    if (!yyin) {
        perror("fopen input");
        return EXIT_FAILURE;
    }
    // Parse all lines (first pass collects labels & emits with placeholders)
    if (yyparse() != 0) {
        fprintf(stderr, "Assembly failed due to parse errors\n");
        return EXIT_FAILURE;
    }
    fclose(yyin);

    // Write out .bin with resolved labels
    finalize_output(argv[2]);

    // Clean up label table
    free_symbols();
    printf("File assembled -> %s\n", argv[2]);
    return EXIT_SUCCESS;
}
