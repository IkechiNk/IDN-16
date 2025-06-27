#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <stdint.h>
#define MAX_SYMBOL 8192

typedef struct Symbol {
    char* name;
    int value;
    int type;
} Symbol;

// Insert a label/assignment with its associated value (addresses are in instruction words)
void insert_symbol(const char* name, int value, int type);

// Sort the list of labels/assignments  
void sort_symbols(void);

// Look up a previously inserted label/assignment and returns its value and type; exits on undefined label/assignment
int get_symbol(const char* name, int* type);

// Free all label storage (call at end of assembly)
void free_symbols(void);


#endif // SYMBOL_TABLE_H
