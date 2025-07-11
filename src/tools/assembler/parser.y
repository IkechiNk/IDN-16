%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "idn16/codegen.h"
#include "idn16/symbol_table.h"

extern int yylineno;
extern char current_token[256];
extern char current_line[1024];
extern int token_idx;
extern int pc;  // Current PC from lexer

int yylex(void);
void yyerror(const char *s);
%}

%union {
    int reg;
    uint16_t num;
    uint16_t pc;

    struct {
      int count;
      uint16_t start_pc; 
    } nops;

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
%token <pc> HLT INC DEC LDB STB
%token <pc> LOAD16 PUSH POP
%token <nops> NOP
%token NEWLINE

%left IMM5 IMM8 OFFSET IMM16
%right NEWLINE
%%
program:
    lines
;

lines:
    /* empty */
  | lines line
;

line:
    label_def NEWLINE
  | assignment NEWLINE
  | instruction NEWLINE
  | NEWLINE        /* Allow empty lines */
;

label_def:
    IDENTIFIER ':'                      { insert_symbol($1.name, pc, 0); free($1.name); }
;

assignment:
    IDENTIFIER '=' IMM16                { insert_symbol($1.name, ((uint16_t)$3.upper << 8) | $3.lower, 1); free($1.name); }
  | IDENTIFIER '=' OFFSET               { insert_symbol($1.name, $3, 1); free($1.name); }
  | IDENTIFIER '=' IMM8                 { insert_symbol($1.name, $3, 1); free($1.name); }
  | IDENTIFIER '=' IMM5                 { insert_symbol($1.name, $3, 1); free($1.name); }
;

instruction:
    ADD  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00000, $2, $4, $6, 0); }
  | SUB  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00001, $2, $4, $6, 0); }
  | AND  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00010, $2, $4, $6, 0); }
  | OR   REG ',' REG ',' REG                        { emit_reg_format($1, 0b00011, $2, $4, $6, 0); }
  | XOR  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00100, $2, $4, $6, 0); }
  | SHL  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00101, $2, $4, $6, 0); }
  | SHR  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00110, $2, $4, $6, 0); } // func=00
  | SRA  REG ',' REG ',' REG                        { emit_reg_format($1, 0b00110, $2, $4, $6, 1); } // func=01
  | MOV  REG ',' REG                                { emit_reg_format($1, 0b00111, $2, $4, 0, 0); } // func=00
  | CMP  REG ',' REG                                { emit_reg_format($1, 0b00111, $2, $4, 0, 1); } // func=01
  | NOT  REG ',' REG                                { emit_reg_format($1, 0b00111, $2, $4, 0, 2); } // func=10
  
  | LDI  REG ',' IMM8                               { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); }
  | LDI  REG ',' IMM5                               { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); }
  | LDI  REG ',' IDENTIFIER                         { emit_imm_format_identifier($1, 0b01000, $2, 0, $4.name, yylineno); }
  | LDW   REG ',' '[' REG '+' IMM5 ']'              { emit_imm_format_with_line($1, 0b01001, $2, $5, $7, yylineno); }
  | LDW   REG ',' '[' REG '+' IDENTIFIER ']'        { emit_imm_format_identifier($1, 0b01001, $2, $5, $7.name, yylineno); }
  | LDW   REG ',' '[' REG ']'                       { emit_imm_format_with_line($1, 0b01001, $2, $5, 0, yylineno); }
  | STW   REG ',' '[' REG '+' IMM5 ']'              { emit_imm_format_with_line($1, 0b01010, $2, $5, $7, yylineno); }
  | STW   REG ',' '[' REG '+' IDENTIFIER ']'        { emit_imm_format_identifier($1, 0b01010, $2, $5, $7.name, yylineno); }
  | STW   REG ',' '[' REG ']'                       { emit_imm_format_with_line($1, 0b01010, $2, $5, 0, yylineno); }
  | ADDI REG ',' REG ',' IMM5                       { emit_imm_format_with_line($1, 0b01011, $2, $4, $6, yylineno); }
  | ADDI REG ',' REG ',' IDENTIFIER                 { emit_imm_format_identifier($1, 0b01011, $2, $4, $6.name, yylineno); }
  | LUI  REG ',' IMM5                               { emit_imm_format_with_line($1, 0b01100, $2, 0, $4, yylineno); }
  | LUI  REG ',' IMM8                               { emit_imm_format_with_line($1, 0b01100, $2, 0, $4, yylineno); }
  | LUI  REG ',' IDENTIFIER                         { emit_imm_format_identifier($1, 0b01100, $2, 0, $4.name, yylineno); }
  | ANDI REG ',' REG ',' IMM5                       { emit_imm_format_with_line($1, 0b01101, $2, $4, $6, yylineno); }
  | ANDI REG ',' REG ',' IDENTIFIER                 { emit_imm_format_identifier($1, 0b01101, $2, $4, $6.name, yylineno); }
  | ORI  REG ',' REG ',' IMM5                       { emit_imm_format_with_line($1, 0b01110, $2, $4, $6, yylineno); }
  | ORI  REG ',' REG ',' IDENTIFIER                 { emit_imm_format_identifier($1, 0b01110, $2, $4, $6.name, yylineno); }
  | XORI REG ',' REG ',' IMM5                       { emit_imm_format_with_line($1, 0b01111, $2, $4, $6, yylineno); }
  | XORI REG ',' REG ',' IDENTIFIER                 { emit_imm_format_identifier($1, 0b01111, $2, $4, $6.name, yylineno); }
  
  | JMP  IDENTIFIER                                 { emit_jb_format_identifier($1, 0b10000, $2.name); }
  | JMP  IMM5                                       { emit_jb_format_imm($1, 0b10000, $2); }
  | JMP  IMM8                                       { emit_jb_format_imm($1, 0b10000, $2); }
  | JMP  OFFSET                                     { emit_jb_format_imm($1, 0b10000, $2); }
  | JEQ  IDENTIFIER                                 { emit_jb_format_identifier($1, 0b10001, $2.name); }
  | JEQ  IMM5                                       { emit_jb_format_imm($1, 0b10001, $2); }
  | JEQ  IMM8                                       { emit_jb_format_imm($1, 0b10001, $2); }
  | JEQ  OFFSET                                     { emit_jb_format_imm($1, 0b10001, $2); }
  | JNE  IDENTIFIER                                 { emit_jb_format_identifier($1, 0b10010, $2.name); }
  | JNE  IMM5                                       { emit_jb_format_imm($1, 0b10010, $2); }
  | JNE  IMM8                                       { emit_jb_format_imm($1, 0b10010, $2); }
  | JNE  OFFSET                                     { emit_jb_format_imm($1, 0b10010, $2); }
  | JGT  IDENTIFIER                                 { emit_jb_format_identifier($1, 0b10011, $2.name); }
  | JGT  IMM5                                       { emit_jb_format_imm($1, 0b10011, $2); }
  | JGT  IMM8                                       { emit_jb_format_imm($1, 0b10011, $2); }
  | JGT  OFFSET                                     { emit_jb_format_imm($1, 0b10011, $2); }
  | JLT  IDENTIFIER                                 { emit_jb_format_identifier($1, 0b10100, $2.name); }
  | JLT  IMM5                                       { emit_jb_format_imm($1, 0b10100, $2); }
  | JLT  IMM8                                       { emit_jb_format_imm($1, 0b10100, $2); }
  | JLT  OFFSET                                     { emit_jb_format_imm($1, 0b10100, $2); }
  | JSR  REG                                        { emit_reg_format($1, 0b10101, $2, 0, 0, 0); }
  | RET                                             { emit_jb_format_imm($1, 0b10110, 0); }
  
  | HLT                                             { emit_special_format($1, 0b11000, 0, 0, 0); }
  | NOP                                             { 
                                                      for (int i = 0; i < $1.count; i++) {
                                                        emit_special_format($1.start_pc + (i * 2), 0b11001, 0, 0, 0);
                                                      }
                                                    }  
  
  | INC  REG                                      { emit_special_format($1, 0b11010, $2, 0, 0); }
  | DEC  REG                                      { emit_special_format($1, 0b11011, $2, 0, 0); }
  | STB REG ',' '[' REG '+' IMM5 ']'              { emit_special_format($1, 0b11101, $2, $5, $7); }
  | STB REG ',' '[' REG '+' IDENTIFIER ']'        { emit_special_format_identifier($1, 0b11101, $2, $5, $7.name, yylineno); }
  | STB REG ',' '[' REG IMM5 ']'                  { emit_special_format($1, 0b11101, $2, $5, $6); }
  | STB REG ',' '[' REG '-' IDENTIFIER ']'        { emit_special_format_neg_identifier($1, 0b11101, $2, $5, $7.name, yylineno); }
  | STB REG ',' '[' REG ']'                       { emit_special_format($1, 0b11101, $2, $5, 0); }
  | LDB REG ',' '[' REG '+' IMM5 ']'              { emit_special_format($1, 0b11100, $2, $5, $7); }
  | LDB REG ',' '[' REG '+' IDENTIFIER ']'        { emit_special_format_identifier($1, 0b11100, $2, $5, $7.name, yylineno); }
  | LDB REG ',' '[' REG IMM5 ']'                  { emit_special_format($1, 0b11100, $2, $5, $6); }
  | LDB REG ',' '[' REG '-' IDENTIFIER ']'        { emit_special_format_neg_identifier($1, 0b11100, $2, $5, $7.name, yylineno); }
  | LDB REG ',' '[' REG ']'                       { emit_special_format($1, 0b11100, $2, $5, 0); }
  
  | LOAD16 REG ',' IMM16                          { emit_imm_format_with_line($1, 0b01000, $2, 0, $4.lower, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, $4.upper, yylineno);}
  | LOAD16 REG ',' OFFSET                         { emit_imm_format_with_line($1, 0b01000, $2, 0, $4 & 0xFF, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, $4 >> 8, yylineno);}
  | LOAD16 REG ',' IMM8                           { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, 0, yylineno);}
  | LOAD16 REG ',' IMM5                           { emit_imm_format_with_line($1, 0b01000, $2, 0, $4, yylineno); emit_imm_format_with_line($1 + 2, 0b01100, $2, 0, 0, yylineno);}
  | LOAD16 REG ',' IDENTIFIER                     { emit_load16_identifier($1, $2, $4.name, yylineno); }
  
  | PUSH REG                                      { emit_push_pseudo($1, $2); }
  | POP REG                                       { emit_pop_pseudo($1, $2); }
;

%%

void yyerror(const char *s) {
    fprintf(stderr, "Error at line %d: %s\n", yylineno, s);
    fprintf(stderr, "Invalid token: %s\n", current_token);
}