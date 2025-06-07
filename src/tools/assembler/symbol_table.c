#include "idn16/symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


static Symbol* symbols[MAX_SYMBOL];
int symbol_cnt = 0;

void insert_symbol(const char* name, int address, int type) {
    // Check for duplicates maybe? need hashmap
    Symbol* symbol = malloc(sizeof(Symbol));
    if (!symbol) { 
        perror("malloc"); 
        exit(1); 
    }
    symbol->name = strdup(name);
    symbol->value = address;
    symbol->type = type;
    symbols[symbol_cnt++] = symbol;
}

int comp(const void *a, const void *b) {
    Symbol* s1 = *(Symbol**)a;
    Symbol* s2 = *(Symbol**)b;
    return strcmp(s1->name, s2->name);
}

// Comparison function for qsort - compares two Symbol pointers
int symbol_compare(const void *a, const void *b) {
    Symbol* s1 = *(Symbol**)a;
    Symbol* s2 = *(Symbol**)b;
    return strcmp(s1->name, s2->name);
}

// Comparison function for bsearch - compares a string key with a Symbol pointer
int symbol_search(const void *key, const void *element) {
    const char* name = (const char*)key;
    Symbol* symbol = *(Symbol**)element;
    return strcmp(name, symbol->name);
}

void sort_symbols(void) {
    qsort(symbols, symbol_cnt, sizeof(Symbol*), symbol_compare);
}

int get_symbol(const char* name, int* type) {
    Symbol** found = (Symbol**)bsearch(name, symbols, symbol_cnt, sizeof(Symbol*), symbol_search);
    if (found) {
        if (type) {
            *type = (*found)->type;
        }
        return (*found)->value;
    }
    fprintf(stderr, "Error: Undefined symbol '%s'\n", name);
    exit(1);
}

void free_symbols(void) {
    for (int i = 0; i < symbol_cnt; i++) {
        Symbol* s = symbols[i];
        if (s) {  // Changed from !l 
            if (s->name) {
                free(s->name);
            }
            free(s);
        }
    }
    symbol_cnt = 0;  // Reset the symbol count
}