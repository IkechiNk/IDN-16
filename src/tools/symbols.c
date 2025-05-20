#include "symbols.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct { 
    char* name; 
    int addr; 
} Label;



Label label_table[128];
int label_count = 0;

void add_label(const char* name, int address) {
    label_table[label_count].name = strdup(name);
    label_table[label_count].addr = address;
    label_count++;
}

int get_label(const char* name) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(label_table[i].name, name) == 0)
            return label_table[i].addr;
    }
    fprintf(stderr, "Undefined label: %s\n", name);
    exit(1);
}