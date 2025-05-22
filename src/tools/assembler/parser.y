%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "codegen.h"
#include "symbol_table.h"

extern int yylineno;
extern char current_token[256];
extern char current_line[1024];
extern int token_idx;

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int reg;
    uint16_t num;
    
    struct {
      char* name;
      int addr;
    } label;
}

%token <reg> REG
%token <num> OFFSET
%token <num> IMM5
%token <num> IMM8
%token <label> LABEL
%token <label> IDENTIFIER

%token ADD SUB AND OR XOR SHL SHR SRA
%token MOV CMP NOT
%token LDI LDW STW ADDI LUI ANDI ORI XORI
%token JMP JEQ JNE JGT JLT JSR RET
%token HLT NOP INC DEC PUSH POP LDB STB

%left IMM5 IMM8 OFFSET
%left IDENTIFIER 
%left LABEL
%%
program:
    lines
;

lines:
    /* empty */
  | lines line
;

line:
    LABEL { 
      insert_label($1.name, $1.addr); 
      free($1.name);
    }
  | instruction
;

instruction:
    ADD  REG ',' REG ',' REG            { emit_reg_format(0b00000, $2, $4, $6, 0); }
  | SUB  REG ',' REG ',' REG            { emit_reg_format(0b00001, $2, $4, $6, 0); }
  | AND  REG ',' REG ',' REG            { emit_reg_format(0b00010, $2, $4, $6, 0); }
  | OR   REG ',' REG ',' REG            { emit_reg_format(0b00011, $2, $4, $6, 0); }
  | XOR  REG ',' REG ',' REG            { emit_reg_format(0b00100, $2, $4, $6, 0); }
  | SHL  REG ',' REG ',' REG            { emit_reg_format(0b00101, $2, $4, $6, 0); }
  | SHR  REG ',' REG ',' REG            { emit_reg_format(0b00110, $2, $4, $6, 0); } // func=00
  | SRA  REG ',' REG ',' REG            { emit_reg_format(0b00110, $2, $4, $6, 1); } // func=01

  | MOV  REG ',' REG                    { emit_reg_format(0b00111, $2, $4, 0, 0); } // func=00
  | CMP  REG ',' REG                    { emit_reg_format(0b00111, $2, $4, 0, 1); } // func=01
  | NOT  REG ',' REG                    { emit_reg_format(0b00111, $2, $4, 0, 2); } // func=10

  | LDI  REG ',' IMM8                   { emit_imm_format(0b01000, $2, 0, $4); }
  | LDI  REG ',' IMM5                   { emit_imm_format(0b01000, $2, 0, $4); }
  | LDW   REG ',' '[' REG '+' IMM5 ']'  { emit_imm_format(0b01001, $2, $5, $7); }
  | STW   REG ',' '[' REG '+' IMM5 ']'  { emit_imm_format(0b01010, $2, $5, $7); }
  | ADDI REG ',' REG ',' IMM5           { emit_imm_format(0b01011, $2, $4, $6); }
  | LUI  REG ',' IMM5                   { emit_imm_format(0b01100, $2, 0, $4); }
  | LUI  REG ',' IMM8                   { emit_imm_format(0b01100, $2, 0, $4); }
  | ANDI REG ',' REG ',' IMM5           { emit_imm_format(0b01101, $2, $4, $6); }
  | ORI  REG ',' REG ',' IMM5           { emit_imm_format(0b01110, $2, $4, $6); }
  | XORI REG ',' REG ',' IMM5           { emit_imm_format(0b01111, $2, $4, $6); }

  | JMP  IDENTIFIER                     { emit_jb_format_label(0b10000, $2.name); }
  | JMP  IMM5                           { emit_jb_format_imm(0b10000, $2); }
  | JMP  IMM8                           { emit_jb_format_imm(0b10000, $2); }
  | JMP  OFFSET                         { emit_jb_format_imm(0b10000, $2); }
  | JEQ  IDENTIFIER                     { emit_jb_format_label(0b10001, $2.name); }
  | JEQ  IMM5                           { emit_jb_format_imm(0b10001, $2); }
  | JEQ  IMM8                           { emit_jb_format_imm(0b10001, $2); }
  | JEQ  OFFSET                         { emit_jb_format_imm(0b10001, $2); }
  | JNE  IDENTIFIER                     { emit_jb_format_label(0b10010, $2.name); }
  | JNE  IMM5                           { emit_jb_format_imm(0b10010, $2); }
  | JNE  IMM8                           { emit_jb_format_imm(0b10010, $2); }
  | JNE  OFFSET                         { emit_jb_format_imm(0b10010, $2); }
  | JGT  IDENTIFIER                     { emit_jb_format_label(0b10011, $2.name); }
  | JGT  IMM5                           { emit_jb_format_imm(0b10011, $2); }
  | JGT  IMM8                           { emit_jb_format_imm(0b10011, $2); }
  | JGT  OFFSET                         { emit_jb_format_imm(0b10011, $2); }
  | JLT  IDENTIFIER                     { emit_jb_format_label(0b10100, $2.name); }
  | JLT  IMM5                           { emit_jb_format_imm(0b10100, $2); }
  | JLT  IMM8                           { emit_jb_format_imm(0b10100, $2); }
  | JLT  OFFSET                         { emit_jb_format_imm(0b10100, $2); }
  | JSR  IDENTIFIER                     { emit_jb_format_label(0b10101, $2.name); }
  | JSR  IMM5                           { emit_jb_format_imm(0b10101, $2); }
  | JSR  IMM8                           { emit_jb_format_imm(0b10101, $2); }
  | JSR  OFFSET                         { emit_jb_format_imm(0b10101, $2); }
  | RET                                 { emit_jb_format_imm(0b10110, 0); }

  | HLT                                 { emit_special_format(0b11000, 0, 0, 0); }
  | NOP                                 { emit_special_format(0b11001, 0, 0, 0); }
  | INC  REG                            { emit_special_format(0b11010, $2, 0, 0); }
  | DEC  REG                            { emit_special_format(0b11011, $2, 0, 0); }
  | PUSH REG                            { emit_special_format(0b11100, $2, 0, 0); }
  | POP  REG                            { emit_special_format(0b11101, $2, 0, 0); }
  | LDB   REG ',' '[' REG '+' IMM5 ']'  { emit_special_format(0b11110, $2, $5, $7); }
  | STB   REG ',' '[' REG '+' IMM5 ']'  { emit_special_format(0b11111, $2, $5, $7); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "Invalid token: %s\n", current_token);
}
