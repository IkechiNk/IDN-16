#include "stdint.h"

// Maximum file size (64KB)
#define MAX_BYTES 65536  

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

/*
 * Internal method to dissasemble a rom file.
*/
int internal_disassemble(const char* filein, const char* fileout);

