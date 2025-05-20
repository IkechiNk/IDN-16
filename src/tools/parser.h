#ifndef PARSER_H
#define PARSER_H

#include <stdint.h>
#include "core/instructions.h"

typedef struct {
    const char* name;
    uint8_t     opcode;
    uint8_t     funct;
    int         argc;
    FormatType  fmt;
    char*       args[3];
} Parsed;

Parsed parse_tokens(char** toks, int count);

Instruction* find_instr(const char* name);

void free_parsed(Parsed *p);

#endif // PARSER_H