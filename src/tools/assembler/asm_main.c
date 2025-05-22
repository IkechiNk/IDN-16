#include <stdio.h>
#include <stdlib.h>

#include "symbol_table.h"
#include "codegen.h"

// Flex/Bison externs
extern FILE* yyin;
int yyparse(void);

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s input.asm output.bin\n", argv[0]);
        return EXIT_FAILURE;
    }

    yyin = fopen(argv[1], "r");
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
    free_labels();
    return EXIT_SUCCESS;
}
