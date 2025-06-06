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
    uint16_t pc;

    struct {
      uint8_t lower;
      uint8_t upper;
    } imm16;
    
    struct {
      char* name;
      int addr;
    } label;
}

%token <reg> REG
%token <num> OFFSET
%token <num> IMM5
%token <num> IMM8
%token <imm16> IMM16
%token <label> IDENTIFIER

%token <pc> ADD SUB AND OR XOR SHL SHR SRA
%token <pc> MOV CMP NOT
%token <pc> LDI LDW STW ADDI LUI ANDI ORI XORI
%token <pc> JMP JEQ JNE JGT JLT JSR RET
%token <pc> HLT NOP INC DEC LDB STB
%token <pc> LOAD16
%token NEWLINE

%left IMM5 IMM8 OFFSET IMM16
%%
program:
    lines
;

lines:
    /* empty */
  | lines line
;

line:
    label_def
  | assignment
  | instruction
  | NEWLINE        /* Allow empty lines */
;

label_def:
    IDENTIFIER ':' opt_newline          { insert_symbol($1.name, $1.addr, 0); free($1.name); }
;

assignment:
    IDENTIFIER '=' IMM16 opt_newline    { insert_symbol($1.name, ((uint16_t)$3.upper << 8) | $3.lower, 1); free($1.name); }
  | IDENTIFIER '=' OFFSET opt_newline   { insert_symbol($1.name, $3, 1); free($1.name); }
  | IDENTIFIER '=' IMM8 opt_newline     { insert_symbol($1.name, $3, 1); free($1.name); }
  | IDENTIFIER '=' IMM5 opt_newline     { insert_symbol($1.name, $3, 1); free($1.name); }
;

opt_newline:
    /* empty */
  | NEWLINE
;

instruction:
    ADD  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00000, $2, $4, $6, 0); }
  | SUB  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00001, $2, $4, $6, 0); }
  | AND  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00010, $2, $4, $6, 0); }
  | OR   REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00011, $2, $4, $6, 0); }
  | XOR  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00100, $2, $4, $6, 0); }
  | SHL  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00101, $2, $4, $6, 0); }
  | SHR  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00110, $2, $4, $6, 0); } // func=00
  | SRA  REG ',' REG ',' REG NEWLINE    { emit_reg_format($1, 0b00110, $2, $4, $6, 1); } // func=01
  | MOV  REG ',' REG NEWLINE            { emit_reg_format($1, 0b00111, $2, $4, 0, 0); } // func=00
  | CMP  REG ',' REG NEWLINE            { emit_reg_format($1, 0b00111, $2, $4, 0, 1); } // func=01
  | NOT  REG ',' REG NEWLINE            { emit_reg_format($1, 0b00111, $2, $4, 0, 2); } // func=10
  | LDI  REG ',' IMM8 NEWLINE           { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); }
  | LDI  REG ',' IMM5 NEWLINE           { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); }
  | LDI  REG ',' IDENTIFIER NEWLINE     { emit_imm_format_identifier($1, 0b01000, $2, 0, $4.name, yylineno); }
  | LDW   REG ',' '[' REG '+' IMM5 ']' NEWLINE  { emit_imm_format_with_line($1, 0b01001, $2, $5, $7, yylineno); }
  | LDW   REG ',' '[' REG '+' IDENTIFIER ']' NEWLINE  { emit_imm_format_identifier($1, 0b01001, $2, $5, $7.name, yylineno); }
  | LDW   REG ',' '[' REG ']' NEWLINE   { emit_imm_format_with_line($1, 0b01001, $2, $5, 0, yylineno); }
  | STW   REG ',' '[' REG '+' IMM5 ']' NEWLINE  { emit_imm_format_with_line($1, 0b01010, $2, $5, $7, yylineno); }
  | STW   REG ',' '[' REG '+' IDENTIFIER ']' NEWLINE  { emit_imm_format_identifier($1, 0b01010, $2, $5, $7.name, yylineno); }
  | STW   REG ',' '[' REG ']' NEWLINE   { emit_imm_format_with_line($1, 0b01010, $2, $5, 0, yylineno); }
  | ADDI REG ',' REG ',' IMM5 NEWLINE   { emit_imm_format_with_line($1, 0b01011, $2, $4, $6, yylineno); }
  | ADDI REG ',' REG ',' IDENTIFIER NEWLINE { emit_imm_format_identifier($1, 0b01011, $2, $4, $6.name, yylineno); }
  | LUI  REG ',' IMM5 NEWLINE           { emit_imm_format_with_line($1, 0b01100, $2, 0, $4, yylineno); }
  | LUI  REG ',' IMM8 NEWLINE           { emit_imm_format_with_line($1, 0b01100, $2, 0, $4, yylineno); }
  | LUI  REG ',' IDENTIFIER NEWLINE     { emit_imm_format_identifier($1, 0b01100, $2, 0, $4.name, yylineno); }
  | ANDI REG ',' REG ',' IMM5 NEWLINE   { emit_imm_format_with_line($1, 0b01101, $2, $4, $6, yylineno); }
  | ANDI REG ',' REG ',' IDENTIFIER NEWLINE { emit_imm_format_identifier($1, 0b01101, $2, $4, $6.name, yylineno); }
  | ORI  REG ',' REG ',' IMM5 NEWLINE   { emit_imm_format_with_line($1, 0b01110, $2, $4, $6, yylineno); }
  | ORI  REG ',' REG ',' IDENTIFIER NEWLINE { emit_imm_format_identifier($1, 0b01110, $2, $4, $6.name, yylineno); }
  | XORI REG ',' REG ',' IMM5 NEWLINE   { emit_imm_format_with_line($1, 0b01111, $2, $4, $6, yylineno); }
  | XORI REG ',' REG ',' IDENTIFIER NEWLINE { emit_imm_format_identifier($1, 0b01111, $2, $4, $6.name, yylineno); }
  | JMP  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10000, $2.name); }
  | JMP  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10000, $2); }
  | JMP  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10000, $2); }
  | JMP  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10000, $2); }
  | JEQ  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10001, $2.name); }
  | JEQ  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10001, $2); }
  | JEQ  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10001, $2); }
  | JEQ  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10001, $2); }
  | JNE  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10010, $2.name); }
  | JNE  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10010, $2); }
  | JNE  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10010, $2); }
  | JNE  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10010, $2); }
  | JGT  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10011, $2.name); }
  | JGT  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10011, $2); }
  | JGT  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10011, $2); }
  | JGT  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10011, $2); }
  | JLT  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10100, $2.name); }
  | JLT  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10100, $2); }
  | JLT  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10100, $2); }
  | JLT  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10100, $2); }
  | JSR  IDENTIFIER NEWLINE              { emit_jb_format_identifier($1, 0b10101, $2.name); }
  | JSR  IMM5 NEWLINE                    { emit_jb_format_imm($1, 0b10101, $2); }
  | JSR  IMM8 NEWLINE                    { emit_jb_format_imm($1, 0b10101, $2); }
  | JSR  OFFSET NEWLINE                  { emit_jb_format_imm($1, 0b10101, $2); }
  | RET NEWLINE                          { emit_jb_format_imm($1, 0b10110, 0); }
  | HLT NEWLINE                          { emit_special_format($1, 0b11000, 0, 0, 0); }
  | NOP NEWLINE                          { emit_special_format($1, 0b11001, 0, 0, 0); }
  | NOP IDENTIFIER NEWLINE               { 
                                          sort_symbols();
                                          uint16_t value = get_symbol($2.name, NULL);
                                          for (int i = 0; i < value; i++) {
                                            emit_special_format($1 + (i * 2), 0b11001, 0, 0, 0);
                                          }
                                          free($2.name);
                                        }
  | NOP OFFSET NEWLINE                   { 
                                          for (int i = 0; i < $2; i++) {
                                            emit_special_format($1 + (i * 2), 0b11001, 0, 0, 0);
                                          }
                                        }  
  | NOP IMM8 NEWLINE                     { 
                                          for (int i = 0; i < $2; i++) {
                                            emit_special_format($1 + (i * 2), 0b11001, 0, 0, 0);
                                          }
                                        }
  | NOP IMM5 NEWLINE                     { 
                                          for (int i = 0; i < $2; i++) {
                                            emit_special_format($1 + (i * 2), 0b11001, 0, 0, 0);
                                          }
                                        }
  
  | INC  REG NEWLINE                     { emit_special_format($1, 0b11010, $2, 0, 0); }
  | DEC  REG NEWLINE                     { emit_special_format($1, 0b11011, $2, 0, 0); }
  | STB REG ',' '[' REG '+' IMM5 ']' NEWLINE    { emit_special_format($1, 0b11101, $2, $5, $7); }
  | STB REG ',' '[' REG '+' IDENTIFIER ']' NEWLINE    { emit_special_format_identifier($1, 0b11101, $2, $5, $7.name, yylineno); }
  | STB REG ',' '[' REG ']' NEWLINE     { emit_special_format($1, 0b11101, $2, $5, 0); }
  | LDB REG ',' '[' REG '+' IMM5 ']' NEWLINE    { emit_special_format($1, 0b11100, $2, $5, $7); }
  | LDB REG ',' '[' REG '+' IDENTIFIER ']' NEWLINE    { emit_special_format_identifier($1, 0b11100, $2, $5, $7.name, yylineno); }
  | LDB REG ',' '[' REG ']' NEWLINE     { emit_special_format($1, 0b11100, $2, $5, 0); }
  | LOAD16 REG ',' IMM16 NEWLINE    { emit_imm_format_with_line($1, 0b01000, $2, 0, $4.lower, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, $4.upper, yylineno);}
  | LOAD16 REG ',' OFFSET NEWLINE   { emit_imm_format_with_line($1, 0b01000, $2, 0, $4 & 0xFF, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, $4 >> 8, yylineno);}
  | LOAD16 REG ',' IMM8 NEWLINE     { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, 0, yylineno);}
  | LOAD16 REG ',' IMM5 NEWLINE     { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, 0, yylineno);}
  | LOAD16 REG ',' IDENTIFIER NEWLINE    { emit_load16_identifier($1, $2, $4.name, yylineno); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "Invalid token: %s\n", current_token);
}