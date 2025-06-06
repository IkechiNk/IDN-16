#include "Unity/unity.h"
#include "../core/cpu.h"
#include "../core/instructions.h"

static Cpu_t* cpu;

void setUp(void) {
    cpu = cpu_init();
}

void tearDown(void) {}

void test_ADD(void) {
    cpu->r[1] = 2; cpu->r[2] = 3;
    add(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(5, cpu->r[3]);

    // Edge: overflow
    cpu->r[1] = 0xFFFF; cpu->r[2] = 1;
    add(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);
    TEST_ASSERT_TRUE(cpu->flags.c);

    // Edge: negative result overflow
    cpu->r[1] = 0x8000; cpu->r[2] = 0xFFFF;
    add(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x7FFF, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.v);
}

void test_SUB(void) {
    cpu->r[1] = 5; cpu->r[2] = 3;
    sub(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(2, cpu->r[3]);

    // Edge: underflow
    cpu->r[1] = 0; cpu->r[2] = 1;
    sub(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.n);

    // Edge: zero result
    cpu->r[1] = 0x1234; cpu->r[2] = 0x1234;
    sub(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);
}

void test_AND(void) {
    cpu->r[1] = 0xF0F0; cpu->r[2] = 0x0FF0;
    and(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x00F0, cpu->r[3]);

    // Edge: all zeros
    cpu->r[1] = 0x0000; cpu->r[2] = 0x0000;
    and(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);

    // Edge: all ones
    cpu->r[1] = 0xFFFF; cpu->r[2] = 0xFFFF;
    and(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);
}

void test_OR(void) {
    cpu->r[1] = 0xF0F0; cpu->r[2] = 0x0FF0;
    or(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFF0, cpu->r[3]);

    // Edge: all zeros
    cpu->r[1] = 0x0000; cpu->r[2] = 0x0000;
    or(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);

    // Edge: all ones
    cpu->r[1] = 0xFFFF; cpu->r[2] = 0x0000;
    or(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);
}

void test_XOR(void) {
    cpu->r[1] = 0xAAAA; cpu->r[2] = 0x5555;
    xor(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);

    // Edge: same values
    cpu->r[1] = 0x1234; cpu->r[2] = 0x1234;
    xor(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);
}

void test_SHL(void) {
    cpu->r[1] = 0x0001; cpu->r[2] = 4;
    shl(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0010, cpu->r[3]);

    // Edge: shift by 0
    cpu->r[1] = 0x1234; cpu->r[2] = 0;
    shl(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1234, cpu->r[3]);

    // Edge: shift by 16
    cpu->r[1] = 0xFFFF; cpu->r[2] = 16;
    shl(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);
}

void test_SHR(void) {
    cpu->r[1] = 0x8000; cpu->r[2] = 3;
    shr(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->r[3]);

    // Edge: shift by 0
    cpu->r[1] = 0x1234; cpu->r[2] = 0;
    shr(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1234, cpu->r[3]);

    // Edge: shift by 16
    cpu->r[1] = 0xFFFF; cpu->r[2] = 16;
    shr(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[3]);
    TEST_ASSERT_TRUE(cpu->flags.z);
}

void test_SRA(void) {
    cpu->r[1] = 0x8000; cpu->r[2] = 3;
    sra(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xF000, cpu->r[3]);

    // Edge: shift by 0
    cpu->r[1] = 0x8000; cpu->r[2] = 0;
    sra(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x8000, cpu->r[3]);

    // Edge: shift by 16
    cpu->r[1] = 0xFFFF; cpu->r[2] = 16;
    sra(3, 1, 2, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);
}

void test_MOV(void) {
    cpu->r[1] = 0x1234;
    mov(2, 1, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1234, cpu->r[2]);

    // Edge: move to r0 (should not change r0)
    cpu->r[1] = 0xFFFF;
    mov(0, 1, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_CMP(void) {
    cpu->r[1] = 5; cpu->r[2] = 5;
    cmp(1, 2, cpu);
    TEST_ASSERT_TRUE(cpu->flags.z);

    // Edge: negative comparison
    cpu->r[1] = 2; cpu->r[2] = 5;
    cmp(1, 2, cpu);
    TEST_ASSERT_TRUE(cpu->flags.n);

    // Edge: not equal
    cpu->r[1] = 7; cpu->r[2] = 5;
    cmp(1, 2, cpu);
    TEST_ASSERT_FALSE(cpu->flags.z);
}

void test_NOT(void) {
    cpu->r[1] = 0xAAAA;
    not(2, 1, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x5555, cpu->r[2]);

    // Edge: all zeros
    cpu->r[1] = 0x0000;
    not(2, 1, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[2]);
}

void test_LDI(void) {
    ldi(1, 0x7F, cpu); // +127
    TEST_ASSERT_EQUAL_UINT16(0x007F, cpu->r[1]);
    ldi(2, 0x80, cpu); // -128
    TEST_ASSERT_EQUAL_UINT16(0xFF80, cpu->r[2]);
    ldi(3, 0xFF, cpu); // -1
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, cpu->r[3]);
    ldi(0, 0x55, cpu); // r0 stays zero
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_LD_and_ST(void) {
    cpu->r[1] = 0xABCD;
    cpu->r[2] = RAM_START;
    st(1, 2, 0x0F, cpu);
    cpu->r[3] = 0;
    ld(3, 2, 0x0F, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xABCD, cpu->r[3]);
    // // Edge: ld to r0 (should not change r0)
    cpu->r[2] = 0x1234;
    st(2, 1, 0x00, cpu);
    ld(0, 1, 0x00, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_ADDI(void) {
    cpu->r[1] = 10;
    addi(2, 1, 0x1F, cpu); // 0x1F = -1 (5 bits)
    TEST_ASSERT_EQUAL_UINT16(9, cpu->r[2]);
    addi(2, 1, 0x0F, cpu); // 0x0F = 15
    TEST_ASSERT_EQUAL_UINT16(25, cpu->r[2]);
    addi(0, 1, 0x1F, cpu); // r0 stays zero
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_ANDI(void) {
    cpu->r[1] = 0xF0F0;
    andi(2, 1, 0x1F, cpu); // 0x1F = 31
    TEST_ASSERT_EQUAL_UINT16(0xF0F0 & 0x001F, cpu->r[2]);
    andi(0, 1, 0x1F, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_ORI(void) {
    cpu->r[1] = 0xF0F0;
    ori(2, 1, 0x1F, cpu); // 0x1F = 31
    TEST_ASSERT_EQUAL_UINT16(0xF0F0 | 0x001F, cpu->r[2]);
    ori(0, 1, 0x1F, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_XORI(void) {
    cpu->r[1] = 0xAAAA;
    xori(2, 1, 0x1F, cpu); // 0x1F = 31
    TEST_ASSERT_EQUAL_UINT16(0xAAAA ^ 0x001F, cpu->r[2]);
    xori(0, 1, 0x1F, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

void test_JMP(void) {
    cpu->pc = 0x1000;
    jmp(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);

    // Edge: negative offset (simulate sign extension)
    cpu->pc = 0x1000;
    jmp((uint16_t)-0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0FF0, cpu->pc);
}

void test_JEQ(void) {
    cpu->pc = 0x1000; cpu->flags.z = 1;
    jeq(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);

    // Edge: not taken
    cpu->pc = 0x1000; cpu->flags.z = 0;
    jeq(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);
}

void test_JNE(void) {
    cpu->pc = 0x1000; cpu->flags.z = 0;
    jne(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);

    // Edge: not taken
    cpu->pc = 0x1000; cpu->flags.z = 1;
    jne(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);
}

void test_JGT(void) {
    cpu->pc = 0x1000; cpu->flags.z = 0; cpu->flags.n = 0;
    jgt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);

    // Edge: not taken (z set)
    cpu->pc = 0x1000; cpu->flags.z = 1; cpu->flags.n = 0;
    jgt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);

    // Edge: not taken (n set)
    cpu->pc = 0x1000; cpu->flags.z = 0; cpu->flags.n = 1;
    jgt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);
}

void test_JLT(void) {
    cpu->pc = 0x1000; cpu->flags.n = 1; cpu->flags.z = 0;
    jlt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);

    // Edge: not taken (z set)
    cpu->pc = 0x1000; cpu->flags.n = 1; cpu->flags.z = 1;
    jlt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);

    // Edge: not taken (n not set)
    cpu->pc = 0x1000; cpu->flags.n = 0; cpu->flags.z = 0;
    jlt(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);
}

void test_JSR_and_RET(void) {
    cpu->pc = 0x1000;
    jsr(0x10, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1010, cpu->pc);
    ret(cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1000, cpu->pc);
}

void test_HLT_and_NOP(void) {
    cpu->running = true;
    hlt(cpu);
    TEST_ASSERT_FALSE(cpu->running);
    cpu->flags.z = 1;
    nop(cpu);
    TEST_ASSERT_EQUAL(0, cpu->flags.z);
}

void test_INC_and_DEC(void) {
    cpu->r[1] = 10;
    inc(1, cpu);
    TEST_ASSERT_EQUAL_UINT16(11, cpu->r[1]);
    dec(1, cpu);
    TEST_ASSERT_EQUAL_UINT16(10, cpu->r[1]);

    // Edge: inc/dec r0 (should not change r0)
    cpu->r[0] = 0;
    inc(0, cpu);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x0000, cpu->r[0], "inc r0");
    dec(0, cpu);
    TEST_ASSERT_EQUAL_UINT16_MESSAGE(0x0000, cpu->r[0], "dec r0");
}

void test_PUSH_and_POP(void) {
    cpu->r[6] = 0x3004; // sp
    cpu->r[1] = 0xBEEF;
    push(1, cpu);
    TEST_ASSERT_EQUAL_HEX16(0x3002, cpu->r[6]);
    cpu->r[1] = 0;
    pop(1, cpu);
    TEST_ASSERT_EQUAL_HEX16(0x3004, cpu->r[6]);
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, cpu->r[1]);

    // Edge: push/pop r0 (should not change r0)
    cpu->r[0] = 0x1234;
    push(0, cpu);
    pop(0, cpu);
    TEST_ASSERT_EQUAL_HEX16(0, cpu->r[0]);
}

void test_LUI(void) {
    lui(1, 0x12, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x1200, cpu->r[1]);
    lui(1, 0xFF, cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFF00, cpu->r[1]);
    // Edge: lui to r0 (should not change r0)
    lui(0, 0x12, cpu);
    TEST_ASSERT_EQUAL_UINT16(0x0000, cpu->r[0]);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_ADD);
    RUN_TEST(test_SUB);
    RUN_TEST(test_AND);
    RUN_TEST(test_OR);
    RUN_TEST(test_XOR);
    RUN_TEST(test_SHL);
    RUN_TEST(test_SHR);
    RUN_TEST(test_SRA);
    RUN_TEST(test_MOV);
    RUN_TEST(test_CMP);
    RUN_TEST(test_NOT);
    RUN_TEST(test_LDI);
    RUN_TEST(test_LD_and_ST);
    RUN_TEST(test_ADDI);
    RUN_TEST(test_ANDI);
    RUN_TEST(test_ORI);
    RUN_TEST(test_XORI);
    RUN_TEST(test_JMP);
    RUN_TEST(test_JEQ);
    RUN_TEST(test_JNE);
    RUN_TEST(test_JGT);
    RUN_TEST(test_JLT);
    RUN_TEST(test_JSR_and_RET);
    RUN_TEST(test_HLT_and_NOP);
    RUN_TEST(test_INC_and_DEC);
    RUN_TEST(test_PUSH_and_POP);
    RUN_TEST(test_LUI);
    return UNITY_END();
}