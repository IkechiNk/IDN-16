#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_LABEL 1024

// Insert a label with its associated address (in instruction words)
void insert_label(const char* name, int address);

// Sort the list of labels
void sort_labels(void);

// Look up a previously inserted label; exits on undefined label
int get_label(const char* name);

// Free all label storage (call at end of assembly)
void free_labels(void);

#endif // SYMBOL_TABLE_H
