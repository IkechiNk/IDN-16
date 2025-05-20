#include "parser.h"
#include "core/instructions.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

// find instruction meta
Instruction* find_instr(const char* name) {
    for (int i = 0; i < INSTR_COUNT; i++)
        if (strcmp(name, INSTRUCTION_SET[i].name) == 0)
            return &INSTRUCTION_SET[i];
    return NULL;
}

Parsed parse_tokens(char** toks, int cnt) {
    Parsed P = {0};
    if (cnt == 0) { P.name = NULL; return P; }

    Instruction* I = find_instr(toks[0]);
    if (!I) {
        fprintf(stderr, "ERR: unknown instr '%s'\n", toks[0]);
        exit(1);
    }
    P.name = I->name;
    P.opcode = I->opcode;
    P.funct = I->funct;
    P.argc = I->arg_count;
    P.fmt = I->fmt;

    for (int i = 0; i < I->arg_count; i++)
        P.args[i] = strdup(toks[i+1]);

    // for LD/ST syntax "[r1+5]" -> args[1]="r1", args[2]="5"
    if (P.fmt == FMT_IMM && strstr(P.args[1], "[") && strstr(P.args[1], "+")) {
        char* b = P.args[1];
        // parse "[r1+imm]" into two separate args
        sscanf(b, "[%3[^+]+%3[^]]]", *P.args, *P.args+1);
    }
    return P;
}

void free_parsed(Parsed *p) {
    for (int i = 0; i < p->argc; i++) {
        free(p->args[i]);
        p->args[i] = NULL;
    }
}