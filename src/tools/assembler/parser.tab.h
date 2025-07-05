/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_HOME_USERNAME_PERSONAL_IDN_16_SRC_TOOLS_ASSEMBLER_PARSER_TAB_H_INCLUDED
# define YY_YY_HOME_USERNAME_PERSONAL_IDN_16_SRC_TOOLS_ASSEMBLER_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    REG = 258,                     /* REG  */
    OFFSET = 259,                  /* OFFSET  */
    IMM5 = 260,                    /* IMM5  */
    IMM8 = 261,                    /* IMM8  */
    IMM16 = 262,                   /* IMM16  */
    IDENTIFIER = 263,              /* IDENTIFIER  */
    ADD = 264,                     /* ADD  */
    SUB = 265,                     /* SUB  */
    AND = 266,                     /* AND  */
    OR = 267,                      /* OR  */
    XOR = 268,                     /* XOR  */
    SHL = 269,                     /* SHL  */
    SHR = 270,                     /* SHR  */
    SRA = 271,                     /* SRA  */
    MOV = 272,                     /* MOV  */
    CMP = 273,                     /* CMP  */
    NOT = 274,                     /* NOT  */
    LDI = 275,                     /* LDI  */
    LDW = 276,                     /* LDW  */
    STW = 277,                     /* STW  */
    ADDI = 278,                    /* ADDI  */
    LUI = 279,                     /* LUI  */
    ANDI = 280,                    /* ANDI  */
    ORI = 281,                     /* ORI  */
    XORI = 282,                    /* XORI  */
    JMP = 283,                     /* JMP  */
    JEQ = 284,                     /* JEQ  */
    JNE = 285,                     /* JNE  */
    JGT = 286,                     /* JGT  */
    JLT = 287,                     /* JLT  */
    JSR = 288,                     /* JSR  */
    RET = 289,                     /* RET  */
    HLT = 290,                     /* HLT  */
    INC = 291,                     /* INC  */
    DEC = 292,                     /* DEC  */
    LDB = 293,                     /* LDB  */
    STB = 294,                     /* STB  */
    LOAD16 = 295,                  /* LOAD16  */
    PUSH = 296,                    /* PUSH  */
    POP = 297,                     /* POP  */
    NOP = 298,                     /* NOP  */
    NEWLINE = 299                  /* NEWLINE  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 19 "/home/username/personal/IDN-16/src/tools/assembler/parser.y"

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

#line 129 "/home/username/personal/IDN-16/src/tools/assembler/parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_HOME_USERNAME_PERSONAL_IDN_16_SRC_TOOLS_ASSEMBLER_PARSER_TAB_H_INCLUDED  */
