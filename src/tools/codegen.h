#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"
#include <stdio.h>

void emit(uint16_t word, FILE* out);

void cg_inst(Parsed* P, int pc, FILE* out);

#endif // CODEGEN_H