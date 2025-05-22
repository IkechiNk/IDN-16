#include "stdint.h"
/*
 * The disassemble_word function decodes word into a human-readable assembly instruction.
 * This is done based on its opcode and format (REG, IMM, JB, or SP). 
 * It returns the instruction form of the word when it is a valid instruction. 
 * On failure, it returns word in hex  
 */
char* disassemble_word(uint16_t word);

/*
 * Reads a word from buffer.
*/
uint16_t read_word(uint8_t *buffer, long i);