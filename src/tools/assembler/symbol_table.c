#include "symbol_table.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


typedef struct Label {
    char* name;
    int address;
} Label;

static Label* labels[MAX_LABEL];
int label_cnt = 0;

void insert_label(const char* name, int address) {
    // Check for duplicates maybe? need hashmap
    Label* label = malloc(sizeof(Label));
    if (!label) { 
        perror("malloc"); 
        exit(1); 
    }
    label->name = strdup(name);
    label->address = address;
    labels[label_cnt++] = label;
}

int comp(const void *a, const void *b) {
    Label* l1 = *(Label**)a;
    Label* l2 = *(Label**)b;
    return strcmp(l1->name, l2->name);
}

// Comparison function for qsort - compares two Label pointers
int label_compare(const void *a, const void *b) {
    Label* l1 = *(Label**)a;
    Label* l2 = *(Label**)b;
    return strcmp(l1->name, l2->name);
}

// Comparison function for bsearch - compares a string key with a Label pointer
int label_search(const void *key, const void *element) {
    const char* name = (const char*)key;
    Label* label = *(Label**)element;
    return strcmp(name, label->name);
}

void sort_labels(void) {
    qsort(labels, label_cnt, sizeof(Label*), label_compare);
}

int get_label(const char* name) {
    Label** found = (Label**)bsearch(name, labels, label_cnt, sizeof(Label*), label_search);
    if (found) {
        return (*found)->address;
    }
    fprintf(stderr, "Error: Undefined label '%s'\n", name);
    exit(1);
}

void free_labels(void) {
    for (int i = 0; i < label_cnt; i++) {
        Label* l = labels[i];
        if (l) {  // Changed from !l 
            if (l->name) {
                free(l->name);
            }
            free(l);
        }
    }
}