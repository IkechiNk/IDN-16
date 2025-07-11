%{
#include "parser.tab.h"
#include <string.h>
#include <stdlib.h>

void yyerror(const char *s);
int pc = 0;

/* Variables for better error reporting */
char current_token[256] = "";  /* Current token text */

/* Function to update token information */
void save_token_info() {
    strncpy(current_token, yytext, sizeof(current_token) - 1);
}
%}

%option yylineno

%%

";"[^\n]*   {/* Do nothing with comments */}

"ADD"|"add"          { save_token_info(); yylval.pc = pc; pc +=2; return ADD; }
"SUB"|"sub"          { save_token_info(); yylval.pc = pc; pc +=2; return SUB; }
"AND"|"and"          { save_token_info(); yylval.pc = pc; pc +=2; return AND; }
"OR"|"or"            { save_token_info(); yylval.pc = pc; pc +=2; return OR;  }
"XOR"|"xor"          { save_token_info(); yylval.pc = pc; pc +=2; return XOR; }
"SHL"|"shl"          { save_token_info(); yylval.pc = pc; pc +=2; return SHL; }
"SHR"|"shr"          { save_token_info(); yylval.pc = pc; pc +=2; return SHR; }
"SRA"|"sra"          { save_token_info(); yylval.pc = pc; pc +=2; return SRA; }
"MOV"|"mov"          { save_token_info(); yylval.pc = pc; pc +=2; return MOV; }
"CMP"|"cmp"          { save_token_info(); yylval.pc = pc; pc +=2; return CMP; }
"NOT"|"not"          { save_token_info(); yylval.pc = pc; pc +=2; return NOT; }

"LDI"|"ldi"          { save_token_info(); yylval.pc = pc; pc +=2; return LDI; }
"LDW"|"ldw"          { save_token_info(); yylval.pc = pc; pc +=2; return LDW; }
"STW"|"stw"          { save_token_info(); yylval.pc = pc; pc +=2; return STW; }
"ADDI"|"addi"        { save_token_info(); yylval.pc = pc; pc +=2; return ADDI;}
"LUI"|"lui"          { save_token_info(); yylval.pc = pc; pc +=2; return LUI; }
"ANDI"|"andi"        { save_token_info(); yylval.pc = pc; pc +=2; return ANDI;}
"ORI"|"ori"          { save_token_info(); yylval.pc = pc; pc +=2; return ORI; }
"XORI"|"xori"        { save_token_info(); yylval.pc = pc; pc +=2; return XORI;}

"JMP"|"jmp"          { save_token_info(); yylval.pc = pc; pc +=2; return JMP; }
"JEQ"|"jeq"          { save_token_info(); yylval.pc = pc; pc +=2; return JEQ; }
"JNE"|"jne"          { save_token_info(); yylval.pc = pc; pc +=2; return JNE; }
"JGT"|"jgt"          { save_token_info(); yylval.pc = pc; pc +=2; return JGT; }
"JLT"|"jlt"          { save_token_info(); yylval.pc = pc; pc +=2; return JLT; }
"JSR"|"jsr"          { save_token_info(); yylval.pc = pc; pc +=2; return JSR; }
"CALL"|"call"        { save_token_info(); yylval.pc = pc; pc +=2; return JSR; }
"RET"|"ret"          { save_token_info(); yylval.pc = pc; pc +=2; return RET; }

"HLT"|"hlt"          { save_token_info(); yylval.pc = pc; pc +=2; return HLT; }
("NOP"|"nop")(" "[0-9]*)?         { 
                        save_token_info(); 
                        if (strlen(yytext) > 4) {
                            yylval.nops.count = atoi(yytext+4); 
                        } else {
                            yylval.nops.count = 1;
                        }
                        yylval.nops.start_pc = pc; 
                        pc += yylval.nops.count * 2; 
                        return NOP;
                     }
"INC"|"inc"          { save_token_info(); yylval.pc = pc; pc +=2; return INC; }
"DEC"|"dec"          { save_token_info(); yylval.pc = pc; pc +=2; return DEC; }
"LDB"|"ldb"          { save_token_info(); yylval.pc = pc; pc +=2; return LDB; }
"STB"|"stb"          { save_token_info(); yylval.pc = pc; pc +=2; return STB; }

"LOAD16"|"load16"    { save_token_info(); yylval.pc = pc; pc += 4; return LOAD16; }
"PUSH"|"push"        { save_token_info(); yylval.pc = pc; pc += 4; return PUSH; }
"POP"|"pop"          { save_token_info(); yylval.pc = pc; pc += 4; return POP; }

("r"|"R")[0-7]             { save_token_info(); yylval.reg = atoi(yytext+1); return REG; }
"zero"|"ZERO"        { save_token_info(); yylval.reg = 0; return REG;}
"sp"|"SP"            { save_token_info(); yylval.reg = 6; return REG;}
"ra"|"RA"            { save_token_info(); yylval.reg = 7; return REG;}

"["            { save_token_info(); return '['; }
"]"            { save_token_info(); return ']'; }
","            { save_token_info(); return ','; }
"+"            { save_token_info(); return '+'; }
"="            { save_token_info(); return '='; }
":"            { save_token_info(); return ':'; }


"0"[xX][a-fA-F0-9]+ {
   save_token_info();
   char* hex = yytext + 2;  /* Fixed: don't modify yytext */
   long val = strtol(hex, NULL, 16);
   if (val <= 0b11111) {
      yylval.num = (uint16_t)val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = (uint16_t)val;
      return IMM8;
   } else if (val <= 0b11111111111) {
      yylval.num = (uint16_t)val;
      return OFFSET;
   } else if (val <= 0b1111111111111111) {
      yylval.imm16.upper = (uint16_t)val >> 8;
      yylval.imm16.lower = (uint16_t)val & 0xFF;
      return IMM16; 
   } else {
      fprintf(stderr, "Error: number too large (%li) on line %d\n", val, yylineno);
      exit(1);
   }
}

"0"[bB][01]+ {
   save_token_info();
   char* bin = yytext + 2;
   long val = strtol(bin, NULL, 2);
   if (val <= 0b11111) {
      yylval.num = (uint16_t)val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = (uint16_t)val;
      return IMM8;
   } else if (val <= 0b11111111111) {
      yylval.num = (uint16_t)val;
      return OFFSET;
   } else if (val <= 0b1111111111111111) {
      yylval.imm16.upper = (uint16_t)val >> 8;
      yylval.imm16.lower = (uint16_t)val & 0xFF;
      return IMM16; 
   } else {
      fprintf(stderr, "Error: number too large (%li) on line %d\n", val, yylineno);
      exit(1);
   }
}

-[0-9]+ {
   save_token_info();
   int val = atoi(yytext);
   if ((-16 <= val) && (val <= 0b11111)) {
      yylval.num = (uint16_t)val;
      return IMM5;
   } else if ((-128 <= val) && (val <= 0b11111111)) {
      yylval.num = (uint16_t)val;
      return IMM8;
   } else if ((-1024 <= val) && (val <= 0b11111111111)) {
      yylval.num = (uint16_t)val;
      return OFFSET;
   } else if ((-32768 <= val) && (val <= 0b1111111111111111)) {
      yylval.imm16.upper = (uint16_t)val >> 8;
      yylval.imm16.lower = (uint16_t)val & 0xFF;
      return IMM16; 
   } else {
      fprintf(stderr, "Error: number too large (%d) on line %d\n", val, yylineno);
      exit(1);
   }
}

[0-9]+ {
   save_token_info();
   int val = atoi(yytext);
   if ((0 <= val) && (val <= 0b11111)) {
      yylval.num = (uint16_t)val;
      return IMM5;
   } else if ((0 <= val) && (val <= 0b11111111)) {
      yylval.num = (uint16_t)val;
      return IMM8;
   } else if ((0 <= val) && (val <= 0b11111111111)) {
      yylval.num = (uint16_t)val;
      return OFFSET;
   } else if ((0 <= val) && (val <= 0b1111111111111111)) {
      yylval.imm16.upper = (uint16_t)val >> 8;
      yylval.imm16.lower = (uint16_t)val & 0xFF;
      return IMM16; 
   } else {
      fprintf(stderr, "Error: number too large (%d) on line %d\n", val, yylineno);
      exit(1);
   }
}

'.' {
   save_token_info();
   int val = yytext[1];
   if (val <= 0b11111) {
      yylval.num = (uint16_t)val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = (uint16_t)val;
      return IMM8;
   } else {
      fprintf(stderr, "Error: character too large (%c) on line %d\n", val, yylineno);
      exit(1);
   }
}

[a-zA-Z_][a-zA-Z0-9_]* {
    save_token_info();
    yylval.label.name = strdup(yytext);
    yylval.label.addr = 0;  // Will be resolved during finalization
    return IDENTIFIER;
}

[ \t]+ {/* Do nothing with whitespace */}

\n { return NEWLINE; }

"-"         { save_token_info(); return '-'; }

. { 
    save_token_info(); 
    return yytext[0]; 
}

%%

void reset_lexer(void) {
    // Reset program counter to initial state
    pc = 0;
    
    // Clear current token buffer
    current_token[0] = '\0';
}