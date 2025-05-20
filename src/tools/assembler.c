#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "symbols.h"
#include <stdio.h>
#include <string.h>
#include <ctype.h>

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input.asm> <output.bin>\n", argv[0]);
        return 1;
    }

    FILE* fin = fopen(argv[1], "r");
    if (!fin) { perror("fopen input"); return 1; }
    FILE* fout = fopen(argv[2], "wb");
    if (!fout) { perror("fopen output"); fclose(fin); return 1; }

    char line[128];
    int pc = 0;
    int line_no;

    // ===== PASS 1: collect labels =====
    line_no = 0;
    while (fgets(line, sizeof(line), fin)) {
        line_no++;
        // trim leading whitespace
        char *t = line;
        while (isspace((unsigned char)*t)) t++;
        if (*t == '\0' || *t == ';') continue;

        int cnt;
        char **toks = tokenize_line(t, &cnt, line_no);

        // label-only
        if (cnt == 1) {
            char *tok = toks[0];
            size_t L = strlen(tok);
            if (L > 1 && tok[L-1] == ':') {
                tok[L-1] = '\0';
                add_label(tok, pc);
                free(toks[0]);
                free(toks);
                continue;
            }
        }

        if (cnt > 0) {
            pc += 2;
        }
        for (int i = 0; i < cnt; i++) free(toks[i]);
        free(toks);
    }

    // ===== PASS 2: generate code =====
    rewind(fin);
    pc = 0;
    line_no = 0;
    while (fgets(line, sizeof(line), fin)) {
        line_no++;
        // trim leading whitespace
        char *t = line;
        while (isspace((unsigned char)*t)) t++;
        if (*t == '\0' || *t == ';') continue;

        int cnt;
        char **toks = tokenize_line(t, &cnt, line_no);

        // skip label-only
        if (cnt == 1) {
            char *tok = toks[0];
            size_t L = strlen(tok);
            if (L > 1 && tok[L-1] == ':') {
                free(toks[0]);
                free(toks);
                continue;
            }
        }

        if (cnt > 0) {
            Parsed P = parse_tokens(toks, cnt);
            cg_inst(&P, pc, fout);
            free_parsed(&P);
            pc += 2;
        }
        for (int i = 0; i < cnt; i++) free(toks[i]);
        free(toks);
    }

    fclose(fin);
    fclose(fout);
    printf("→ Assembled %d instructions into %s\n", pc/2, argv[2]);
    return 0;
}
