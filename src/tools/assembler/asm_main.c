#include <stdio.h>
#include <stdlib.h>

#include "idn16/symbol_table.h"
#include "idn16/codegen.h"
#include "idn16/asmblr.h"

// Flex/Bison externs
extern FILE* yyin;
extern int yylineno;
extern int pc;
extern char current_token[256];
int yyparse(void);
int yylex_destroy(void);
void yyrestart(FILE *input_file);

void add_newline_to_file(const char* filename) {
    FILE *f = fopen(filename, "rb+");
    if (!f) {
        fprintf(stderr, "Error: Unable to open the file \"%s\" to add a new line\n", filename);
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

int internal_assemble(const char* filein, const char* fileout) {
    // Reset all static variables to initial state
    reset_codegen();
    reset_lexer();
    
    add_newline_to_file(filein);
    yyin = fopen(filein, "r");
    if (!yyin) {
        perror("fopen input");
        return EXIT_FAILURE;
    }
    
    // Reset flex/bison state
    yylineno = 1;
    yyrestart(yyin);
    
    // Parse all lines (first pass collects labels & emits with placeholders)
    if (yyparse() != 0) {
        fprintf(stderr, "Assembly failed due to parse errors\n");
        fclose(yyin);
        yylex_destroy();
        return EXIT_FAILURE;
    }
    fclose(yyin);
    
    // Clean up lexer
    yylex_destroy();

    // Write out .bin with resolved labels
    finalize_output(fileout);

    // Clean up label table
    free_symbols();
    
    printf("File assembled -> %s\n", fileout);
    return EXIT_SUCCESS;
}


