#include "Unity/unity.h"
#include "../tools/dasm.h"
#include <string.h>

void setUp(void) {
    // Note: pc is static in dasm.c and cannot be reset from here
}

void tearDown(void) {}

// Test REG-format instructions
void test_disassemble_add_instruction(void) {
    // ADD r1, r2, r3 - opcode=0, rd=1, rs1=2, rs2=3, func=0
    uint16_t word = (0 << 11) | (1 << 8) | (2 << 5) | (3 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("ADD  r1, r2, r3\n", result);
}

void test_disassemble_sub_instruction(void) {
    // SUB r0, r1, r2 - opcode=1, rd=0, rs1=1, rs2=2, func=0
    uint16_t word = (1 << 11) | (0 << 8) | (1 << 5) | (2 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("SUB  r0, r1, r2\n", result);
}

void test_disassemble_and_instruction(void) {
    // AND r3, r4, r5 - opcode=2, rd=3, rs1=4, rs2=5, func=0
    uint16_t word = (2 << 11) | (3 << 8) | (4 << 5) | (5 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("AND  r3, r4, r5\n", result);
}

void test_disassemble_or_instruction(void) {
    // OR r2, r3, r4 - opcode=3
    uint16_t word = (3 << 11) | (2 << 8) | (3 << 5) | (4 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("OR   r2, r3, r4\n", result);
}

void test_disassemble_xor_instruction(void) {
    // XOR r1, r2, r3 - opcode=4
    uint16_t word = (4 << 11) | (1 << 8) | (2 << 5) | (3 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("XOR  r1, r2, r3\n", result);
}

void test_disassemble_shl_instruction(void) {
    // SHL r1, r2, r3 - opcode=5
    uint16_t word = (5 << 11) | (1 << 8) | (2 << 5) | (3 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("SHL  r1, r2, r3\n", result);
}

void test_disassemble_shr_instruction(void) {
    // SHR r1, r2, r3 - opcode=6, func=0
    uint16_t word = (6 << 11) | (1 << 8) | (2 << 5) | (3 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("SHR  r1, r2, r3\n", result);
}

void test_disassemble_sra_instruction(void) {
    // SRA r1, r2, r3 - opcode=6, func=1
    uint16_t word = (6 << 11) | (1 << 8) | (2 << 5) | (3 << 2) | 1;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("SRA  r1, r2, r3\n", result);
}

void test_disassemble_mov_instruction(void) {
    // MOV r1, r2 - opcode=7, func=0
    uint16_t word = (7 << 11) | (1 << 8) | (2 << 5) | (0 << 2) | 0;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("MOV  r1, r2\n", result);
}

void test_disassemble_cmp_instruction(void) {
    // CMP r1, r2 - opcode=7, func=1
    uint16_t word = (7 << 11) | (1 << 8) | (2 << 5) | (0 << 2) | 1;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("CMP  r1, r2\n", result);
}

void test_disassemble_not_instruction(void) {
    // NOT r1, r2 - opcode=7, func=2
    uint16_t word = (7 << 11) | (1 << 8) | (2 << 5) | (0 << 2) | 2;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("NOT  r1, r2\n", result);
}

// Test IMM-format instructions
void test_disassemble_ldi_instruction(void) {
    // LDI r1, 0x42 - opcode=8, rd=1, imm=0x42
    uint16_t word = (8 << 11) | (1 << 8) | 0x42;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("LDI  r1, 0x42\n", result);
}

void test_disassemble_ldw_instruction(void) {
    // LDW r1, [r2+5] - opcode=9, rd=1, rs1=2, imm=5
    uint16_t word = (9 << 11) | (1 << 8) | (2 << 5) | 5;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("LDW  r1, [r2+5]\n", result);
}

void test_disassemble_stw_instruction(void) {
    // STW r1, [r2+3] - opcode=10, rd=1, rs1=2, imm=3
    uint16_t word = (10 << 11) | (1 << 8) | (2 << 5) | 3;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("STW  r1, [r2+3]\n", result);
}

void test_disassemble_addi_instruction(void) {
    // ADDI r1, r2, 7 - opcode=11, rd=1, rs1=2, imm=7
    uint16_t word = (11 << 11) | (1 << 8) | (2 << 5) | 7;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("ADDI r1, r2, 7\n", result);
}

void test_disassemble_lui_instruction(void) {
    // LUI r1, 0x34 - opcode=12, rd=1, imm=0x34
    uint16_t word = (12 << 11) | (1 << 8) | 0x34;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("LUI  r1, 0x34\n", result);
}

void test_disassemble_andi_instruction(void) {
    // ANDI r1, r2, 15 - opcode=13, rd=1, rs1=2, imm=15
    uint16_t word = (13 << 11) | (1 << 8) | (2 << 5) | 15;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("ANDI r1, r2, 15\n", result);
}

void test_disassemble_ori_instruction(void) {
    // ORI r1, r2, 8 - opcode=14
    uint16_t word = (14 << 11) | (1 << 8) | (2 << 5) | 8;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("ORI  r1, r2, 8\n", result);
}

void test_disassemble_xori_instruction(void) {
    // XORI r1, r2, 12 - opcode=15
    uint16_t word = (15 << 11) | (1 << 8) | (2 << 5) | 12;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("XORI r1, r2, 12\n", result);
}

void test_disassemble_negative_immediate(void) {
    // ADDI r1, r2, -1 (0x1F = -1 in 5-bit signed)
    uint16_t word = (11 << 11) | (1 << 8) | (2 << 5) | 0x1F;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("ADDI r1, r2, -1\n", result);
}

// Test JB-format instructions
void test_disassemble_jmp_instruction(void) {
    // JMP 100 - opcode=16, offset=100
    uint16_t word = (16 << 11) | 100;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JMP  100\n", result);
}

void test_disassemble_jeq_instruction(void) {
    // JEQ 50 - opcode=17
    uint16_t word = (17 << 11) | 50;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JEQ  50\n", result);
}

void test_disassemble_jne_instruction(void) {
    // JNE 25 - opcode=18
    uint16_t word = (18 << 11) | 25;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JNE  25\n", result);
}

void test_disassemble_jgt_instruction(void) {
    // JGT 75 - opcode=19
    uint16_t word = (19 << 11) | 75;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JGT  75\n", result);
}

void test_disassemble_jlt_instruction(void) {
    // JLT 200 - opcode=20
    uint16_t word = (20 << 11) | 200;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JLT  200\n", result);
}

void test_disassemble_jsr_instruction(void) {
    // JSR 300 - opcode=21
    uint16_t word = (21 << 11) | 300;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JSR  300\n", result);
}

void test_disassemble_ret_instruction(void) {
    // RET - opcode=22
    uint16_t word = (22 << 11);
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("RET\n", result);
}

void test_disassemble_negative_jump_offset(void) {
    // JMP -10 (sign extend 11-bit value)
    uint16_t offset = (-10) & 0x7FF;  // -10 in 11-bit
    uint16_t word = (16 << 11) | offset;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("JMP  -10\n", result);
}

// Test SP-format instructions
void test_disassemble_hlt_instruction(void) {
    // HLT - opcode=24
    uint16_t word = (24 << 11);
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("HLT\n", result);
}

void test_disassemble_nop_instruction(void) {
    // NOP - opcode=25
    uint16_t word = (25 << 11);
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("NOP\n", result);
}

void test_disassemble_inc_instruction(void) {
    // INC r3 - opcode=26, rd=3
    uint16_t word = (26 << 11) | (3 << 8);
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("INC  r3\n", result);
}

void test_disassemble_dec_instruction(void) {
    // DEC r5 - opcode=27, rd=5
    uint16_t word = (27 << 11) | (5 << 8);
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("DEC  r5\n", result);
}

void test_disassemble_ldb_instruction(void) {
    // LDB r1, [r2+7] - opcode=28, rd=1, rs1=2, imm=7
    uint16_t word = (28 << 11) | (1 << 8) | (2 << 5) | 7;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("LDB  r1, [r2+7]\n", result);
}

void test_disassemble_stb_instruction(void) {
    // STB r1, [r2+4] - opcode=29, rd=1, rs1=2, imm=4
    uint16_t word = (29 << 11) | (1 << 8) | (2 << 5) | 4;
    char* result = disassemble_word(word);
    TEST_ASSERT_EQUAL_STRING("STB  r1, [r2+4]\n", result);
}

// Test fallback case
void test_disassemble_unknown_instruction(void) {
    // Invalid opcode (31) - should fallback to .word
    uint16_t word = (31 << 11) | 0x123;
    char* result = disassemble_word(word);
    char expected[32];
    sprintf(expected, ".word 0x%04X\n", word);
    TEST_ASSERT_EQUAL_STRING(expected, result);
}

// Test read_word function
void test_read_word_little_endian(void) {
    uint8_t buffer[] = {0x34, 0x12, 0x78, 0x56};
    uint16_t word = read_word(buffer, 0);
    TEST_ASSERT_EQUAL_HEX16(0x1234, word);
    
    word = read_word(buffer, 2);
    TEST_ASSERT_EQUAL_HEX16(0x5678, word);
}

void test_read_word_edge_cases(void) {
    uint8_t buffer[] = {0x00, 0x00, 0xFF, 0xFF};
    uint16_t word = read_word(buffer, 0);
    TEST_ASSERT_EQUAL_HEX16(0x0000, word);
    
    word = read_word(buffer, 2);
    TEST_ASSERT_EQUAL_HEX16(0xFFFF, word);
}

int main(void) {
    UNITY_BEGIN();
    
    // REG-format tests
    RUN_TEST(test_disassemble_add_instruction);
    RUN_TEST(test_disassemble_sub_instruction);
    RUN_TEST(test_disassemble_and_instruction);
    RUN_TEST(test_disassemble_or_instruction);
    RUN_TEST(test_disassemble_xor_instruction);
    RUN_TEST(test_disassemble_shl_instruction);
    RUN_TEST(test_disassemble_shr_instruction);
    RUN_TEST(test_disassemble_sra_instruction);
    RUN_TEST(test_disassemble_mov_instruction);
    RUN_TEST(test_disassemble_cmp_instruction);
    RUN_TEST(test_disassemble_not_instruction);
    
    // IMM-format tests
    RUN_TEST(test_disassemble_ldi_instruction);
    RUN_TEST(test_disassemble_ldw_instruction);
    RUN_TEST(test_disassemble_stw_instruction);
    RUN_TEST(test_disassemble_addi_instruction);
    RUN_TEST(test_disassemble_lui_instruction);
    RUN_TEST(test_disassemble_andi_instruction);
    RUN_TEST(test_disassemble_ori_instruction);
    RUN_TEST(test_disassemble_xori_instruction);
    RUN_TEST(test_disassemble_negative_immediate);
    
    // JB-format tests
    RUN_TEST(test_disassemble_jmp_instruction);
    RUN_TEST(test_disassemble_jeq_instruction);
    RUN_TEST(test_disassemble_jne_instruction);
    RUN_TEST(test_disassemble_jgt_instruction);
    RUN_TEST(test_disassemble_jlt_instruction);
    RUN_TEST(test_disassemble_jsr_instruction);
    RUN_TEST(test_disassemble_ret_instruction);
    RUN_TEST(test_disassemble_negative_jump_offset);
    
    // SP-format tests
    RUN_TEST(test_disassemble_hlt_instruction);
    RUN_TEST(test_disassemble_nop_instruction);
    RUN_TEST(test_disassemble_inc_instruction);
    RUN_TEST(test_disassemble_dec_instruction);
    RUN_TEST(test_disassemble_ldb_instruction);
    RUN_TEST(test_disassemble_stb_instruction);
    
    // Edge cases
    RUN_TEST(test_disassemble_unknown_instruction);
    RUN_TEST(test_read_word_little_endian);
    RUN_TEST(test_read_word_edge_cases);
    
    return UNITY_END();
}