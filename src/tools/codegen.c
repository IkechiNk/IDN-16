#include "codegen.h"
#include "symbols.h"
#include "core/instructions.h"
#include "core/memory.h"
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

/* encode helpers */
static int regnum(const char* r) { return atoi(r+1) & 0x7; }
static int imm_val(const char* s)  { return atoi(s)     & 0x1F; }
static int imm8(const char* s)     { return atoi(s)     & 0xFF; }

void emit(uint16_t w, FILE* out) {
    fputc((w >> 8) & 0xFF, out);
    fputc(w & 0xFF, out);
}

void cg_inst(Parsed* P, int pc, FILE* out) {
    uint16_t w = P->opcode << 11;

    switch (P->fmt) {
      case FMT_REG: {
        int rd  = regnum(P->args[0]);
        int rs1 = regnum(P->args[1]);
        int rs2 = P->argc == 3 ? regnum(P->args[2]) : 0;
        w |= (rd  & 7) << 8
          | (rs1 & 7) << 5
          | (rs2 & 7) << 2
          | (P->funct & 0x3);
        break;
      }

      case FMT_IMM: {
        int rd  = regnum(P->args[0]);
        int rs1 = regnum(P->args[1]);
        int imm = (P->argc == 2)
                  ? imm8(P->args[1])    // LDI/LUI use full 8 bits
                  : imm_val(P->args[2]); // others use 5-bit imm
        w |= (rd  & 7) << 8
          | (rs1 & 7) << 5
          | (imm & ((P->argc == 2) ? 0xFF : 0x1F));
        if (P->argc == 2) {
            // For LDI/LUI: overwrite top bits properly
            w = (P->opcode << 11) | ((rd & 7) << 8) | (imm & 0xFF);
        }
        break;
      }

      case FMT_JB: {
        int off_words = 0;
        if (P->argc == 1) {
            int label_pc = 0;
            if (isalpha(P->args[0][0])) {
                label_pc = get_label(P->args[0]);
            } else {
                label_pc = atoi(P->args[0]) * 2;  // numeric offset in words → bytes
            }
            // Convert both pcs to word-indices, then subtract PC+1 (next instr)
            int current_word = pc / 2;
            int target_word  = label_pc / 2;
            off_words = target_word - current_word;
            off_words *= 2; // convert to bytes
        }
        // mask to 11 bits (two's-complement)
        off_words &= 0x7FF;
        w |= off_words;
        break;
      }

      case FMT_SP: {
        int rd = (P->argc == 1) ? regnum(P->args[0]) : 0;
        w |= (rd & 7) << 8
          | (P->funct & 0xFF);
        break;
      }
    }

    emit(w, out);
}
