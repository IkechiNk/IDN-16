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
%option case-insensitive

%%

";"[^\n]*   {/* Do nothing with comments */}

"ADD"       { save_token_info(); pc +=2; return ADD; }
"SUB"       { save_token_info(); pc +=2; return SUB; }
"AND"       { save_token_info(); pc +=2; return AND; }
"OR"        { save_token_info(); pc +=2; return OR;  }
"XOR"       { save_token_info(); pc +=2; return XOR; }
"SHL"       { save_token_info(); pc +=2; return SHL; }
"SHR"       { save_token_info(); pc +=2; return SHR; }
"SRA"       { save_token_info(); pc +=2; return SRA; }
"MOV"       { save_token_info(); pc +=2; return MOV; }
"CMP"       { save_token_info(); pc +=2; return CMP; }
"NOT"       { save_token_info(); pc +=2; return NOT; }

"LDI"       { save_token_info(); pc +=2; return LDI; }
"LDW"       { save_token_info(); pc +=2; return LDW; }
"STW"       { save_token_info(); pc +=2; return STW; }
"ADDI"      { save_token_info(); pc +=2; return ADDI;}
"LUI"       { save_token_info(); pc +=2; return LUI; }
"ANDI"      { save_token_info(); pc +=2; return ANDI;}
"ORI"       { save_token_info(); pc +=2; return ORI; }
"XORI"      { save_token_info(); pc +=2; return XORI;}

"JMP"       { save_token_info(); pc +=2; return JMP; }
"JEQ"       { save_token_info(); pc +=2; return JEQ; }
"JNE"       { save_token_info(); pc +=2; return JNE; }
"JGT"       { save_token_info(); pc +=2; return JGT; }
"JLT"       { save_token_info(); pc +=2; return JLT; }
"JSR"       { save_token_info(); pc +=2; return JSR; }
"CALL"      { save_token_info(); pc +=2; return JSR; }
"RET"       { save_token_info(); pc +=2; return RET; }

"HLT"       { save_token_info(); pc +=2; return HLT; }
"NOP"       { save_token_info(); pc +=2; return NOP; }
"INC"       { save_token_info(); pc +=2; return INC; }
"DEC"       { save_token_info(); pc +=2; return DEC; }
"PUSH"      { save_token_info(); pc +=2; return PUSH;}
"POP"       { save_token_info(); pc +=2; return POP; }
"LDB"       { save_token_info(); pc +=2; return LDB; }
"STB"       { save_token_info(); pc +=2; return STB; }

"r"[0-7]    { save_token_info(); yylval.reg = atoi(yytext+1); return REG; }
"zero"      { save_token_info(); yylval.reg = 0; return REG;}
"sp"        { save_token_info(); yylval.reg = 6; return REG;}
"ra"        { save_token_info(); yylval.reg = 7; return REG;}

"0"[xX][a-fA-F0-9]+ {
   save_token_info();
   char* hex = yytext + 2;  /* Fixed: don't modify yytext */
   uint16_t val = (uint16_t)strtol(hex, NULL, 16);
   if (val <= 0b11111) {
      yylval.num = val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = val;
      return IMM8;
   } else if (val <= 0b11111111111) {
      yylval.num = val;
      return OFFSET;
   } else {
      fprintf(stderr, "Error: number too large (%d) on line %d\n", val, yylineno);
      exit(1);
   }
}

"0"[bB][01]+ {
   save_token_info();
   char* bin = yytext + 2;
   uint16_t val = (uint16_t)strtol(bin, NULL, 2);
   if (val <= 0b11111) {
      yylval.num = val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = val;
      return IMM8;
   } else if (val <= 0b11111111111) {
      yylval.num = val;
      return OFFSET;
   } else {
      fprintf(stderr, "Error: number too large (%d) on line %d\n", val, yylineno);
      exit(1);
   }
}

[0-9]+ {
   save_token_info();
   uint16_t val = (uint16_t)atoi(yytext);
   if (val <= 0b11111) {
      yylval.num = val;
      return IMM5;
   } else if (val <= 0b11111111) {
      yylval.num = val;
      return IMM8;
   } else if (val <= 0b11111111111) {
      yylval.num = val;
      return OFFSET;
   } else {
      fprintf(stderr, "Error: number too large (%d) on line %d\n", val, yylineno);
      exit(1);
   }
}

"["         { save_token_info(); return '['; }
"]"         { save_token_info(); return ']'; }
","         { save_token_info(); return ','; }
"+"         { save_token_info(); return '+'; }

[a-zA-Z_][a-zA-Z0-9_]*: {
    save_token_info();
    yylval.label.name = strdup(yytext);
    yylval.label.name[strlen(yytext)-1] = '\0';  // remove colon
    yylval.label.addr = pc;
    return LABEL;
}

[a-zA-Z_][a-zA-Z0-9_]* {
    save_token_info();
    yylval.label.name = strdup(yytext);
    yylval.label.addr = 0;  // Will be resolved during finalization
    return IDENTIFIER;
}

[ \t]+ {/* Do nothing with whitespace */}

\n {/* Do nothing with newline */}

. { 
    save_token_info(); 
    return yytext[0]; 
}

%%