#include "lexer.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define MAX_TOKENS 6

char** tokenize_line(const char* line, int* count, int line_number) {
    char** toks = malloc(sizeof(char*) * MAX_TOKENS);
    if (!toks) {
        perror("malloc");
        exit(1);
    }

    // Make a mutable copy and strip any trailing newline
    char* buf = strdup(line);
    if (!buf) {
        perror("strdup");
        exit(1);
    }
    buf[strcspn(buf, "\r\n")] = '\0';

    // Remove comments
    char* cpos = strchr(buf, ';');
    if (cpos) *cpos = '\0';

    // Tokenize on space, comma, tab
    int n = 0;
    char* tok = strtok(buf, " \t,");
    while (tok) {
        if (n >= MAX_TOKENS) {
            fprintf(stderr,
                    "Error on source line %d: too many tokens: \"%s\"\n",
                    line_number, line);
            // clean up allocated tokens so far
            for (int i = 0; i < n; i++) free(toks[i]);
            free(toks);
            free(buf);
            exit(1);
        }
        toks[n++] = strdup(tok);
        tok = strtok(NULL, " \t,");
    }

    *count = n;
    free(buf);
    return toks;
}