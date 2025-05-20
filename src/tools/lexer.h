#ifndef LEXER_H
#define LEXER_H

/**
 * Splits a line of assembly into tokens.
 *
 * @param  line       The raw input line (may include label, instr, args, comments)
 * @param  count      Output: number of tokens produced
 * @param  pc         The current program counter (byte address) for error messages
 * @return            A malloc’d array of strdup’d strings; caller must free each and then the array.
 */
char** tokenize_line(const char* line, int* token_count, int line_number);

#endif