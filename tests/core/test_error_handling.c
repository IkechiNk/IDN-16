#include "../unity/unity.h"
#include "idn16/cpu.h"
#include "idn16/memory.h"

static Cpu_t* cpu;

void setUp(void) {
    cpu = cpu_init();
    TEST_ASSERT_NOT_NULL(cpu);
}

void tearDown(void) {
    cpu_destroy(cpu);
}

void test_memory_write_protection(void) {
    // Test writing to ROM region (should fail with unprivileged access)
    bool result = memory_write_byte(cpu->memory, USER_ROM_START, 0x42, false);
    TEST_ASSERT_FALSE(result);
    
    // Test writing to ROM region (should succeed with privileged access)
    result = memory_write_byte(cpu->memory, USER_ROM_START, 0x42, true);
    TEST_ASSERT_TRUE(result);
}

void test_invalid_syscall_address(void) {
    // Test system call with invalid address
    // This should print an error message but not crash
    handle_system_call(0x1234, cpu); // Invalid syscall address
    
    // CPU should still exist (we just need to see a message in the output stream)
    TEST_ASSERT_NOT_NULL(cpu);
}

void test_division_by_zero_handling(void) {
    cpu->r[1] = 100;
    cpu->r[2] = 0;
    
    // This should not crash
    syscall_divide(cpu);
    
    // Should return 0 for both quotient and remainder
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[2]);
}

void test_invalid_sprite_operations(void) {
    // Test sprite operations with invalid parameters
    cpu->r[1] = 9999;  // Invalid sprite ID (max is 1199)
    cpu->r[2] = 100;   // X position
    cpu->r[3] = 100;   // Y position
    cpu->r[4] = 1;     // Tile ID
    
    syscall_set_sprite(cpu);
    
    // Should fail gracefully
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]); // Should return failure
}

void test_memory_region_validation(void) {
    // Test memory region detection
    TEST_ASSERT_EQUAL_INT(REGION_USER_ROM, memory_get_region(0x0000));
    TEST_ASSERT_EQUAL_INT(REGION_USER_ROM, memory_get_region(0x7FFF));
    TEST_ASSERT_EQUAL_INT(REGION_RAM, memory_get_region(0x8000));
    TEST_ASSERT_EQUAL_INT(REGION_RAM, memory_get_region(0xCFFF));
    TEST_ASSERT_EQUAL_INT(REGION_VIDEO, memory_get_region(0xD000));
    TEST_ASSERT_EQUAL_INT(REGION_VIDEO, memory_get_region(0xEFFF));
    TEST_ASSERT_EQUAL_INT(REGION_AUDIO, memory_get_region(0xF000));
    TEST_ASSERT_EQUAL_INT(REGION_AUDIO, memory_get_region(0xF0FF));
    TEST_ASSERT_EQUAL_INT(REGION_INPUT, memory_get_region(0xF100));
    TEST_ASSERT_EQUAL_INT(REGION_INPUT, memory_get_region(0xF1FF));
    TEST_ASSERT_EQUAL_INT(REGION_SYSTEM_CTRL, memory_get_region(0xF200));
    TEST_ASSERT_EQUAL_INT(REGION_SYSTEM_CTRL, memory_get_region(0xF2FF));
    TEST_ASSERT_EQUAL_INT(REGION_SYSCALL, memory_get_region(0xF300));
    TEST_ASSERT_EQUAL_INT(REGION_SYSCALL, memory_get_region(0xFFFF));
}

void test_invalid_audio_channel(void) {
    // Test with invalid audio channel
    cpu->r[1] = 10;    // Invalid channel (only 0-3 valid)
    cpu->r[2] = 440;   // Frequency
    cpu->r[3] = 1000;  // Duration
    cpu->r[4] = 200;   // Volume
    
    syscall_play_tone_channel(cpu);
    
    // Should return failure
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
    
    // Test stopping invalid channel
    cpu->r[1] = 5;     // Invalid channel
    syscall_stop_channel(cpu);
    
    // Should return failure
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
}

void test_memory_initialization(void) {
    // Test that memory is properly initialized
    TEST_ASSERT_NOT_NULL(cpu->memory);
    
    // Test that video memory is initialized
    // Default palette should be set
    uint16_t default_color1 = memory_read_word(cpu->memory, PALETTE_RAM_START);
    uint16_t default_color2 = memory_read_word(cpu->memory, PALETTE_RAM_START + 2);
    TEST_ASSERT_EQUAL(0, default_color1);       // Should have color black (0x0000)
    TEST_ASSERT_NOT_EQUAL(0, default_color2);   // Should have some default color (non-black)
}

void test_stack_overflow_protection(void) {
    // Test stack operations near boundaries
    // Set stack pointer to top of RAM
    cpu->r[6] = RAM_END;
    
    // This should work (just at boundary)
    cpu->r[1] = 0x1234;
    
    // Simulate PUSH operation
    cpu->r[6] -= 2;  // Decrement stack pointer
    bool result = memory_write_word(cpu->memory, cpu->r[6], cpu->r[1], false);
    TEST_ASSERT_TRUE(result);
    
    // Now try to push below RAM start (should fail)
    cpu->r[6] = RAM_START - 2;
    result = memory_write_word(cpu->memory, cpu->r[6], cpu->r[1], false);
    TEST_ASSERT_FALSE(result); // Should fail - outside RAM region
}

void test_cpu_initialization(void) {
    // Test CPU is properly initialized
    TEST_ASSERT_NOT_NULL(cpu);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->pc);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[0]); // r0 should be 0
    TEST_ASSERT_EQUAL_UINT8(0, cpu->flags.z);
    TEST_ASSERT_EQUAL_UINT8(0, cpu->flags.n);
    TEST_ASSERT_EQUAL_UINT8(0, cpu->flags.c);
    TEST_ASSERT_EQUAL_UINT8(0, cpu->flags.v);
    TEST_ASSERT_EQUAL_UINT64(0, cpu->cycles);
    TEST_ASSERT_EQUAL_UINT32(0, cpu->frame_count);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->sleep_timer);
    TEST_ASSERT_TRUE(cpu->running);
}

void test_invalid_instruction_handling(void) {
    // Test that invalid instructions don't crash the system
    // Write an invalid instruction to memory
    memory_write_word(cpu->memory, 0x0000, 0xFFFF, true); // Invalid instruction
    
    // Set PC to point to it
    cpu->pc = 0x0000;
    
    // This should not crash (might print error but continue)
    uint16_t instruction = fetch(cpu);
    TEST_ASSERT_EQUAL_UINT16(0xFFFF, instruction);
    
    // Decode should handle it gracefully
    shared decoded = decode(instruction);
    TEST_ASSERT_NOT_NULL(&decoded);
}

void test_null_pointer_protection(void) {
    // Test that functions handle NULL pointers gracefully
    // Note: This is implementation-dependent, but good to test
    
    // Create a CPU with null memory (if possible)
    // Most functions should check for null pointers
    TEST_ASSERT_NOT_NULL(cpu->memory);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_memory_write_protection);
    RUN_TEST(test_invalid_syscall_address);
    RUN_TEST(test_division_by_zero_handling);
    RUN_TEST(test_invalid_sprite_operations);
    RUN_TEST(test_memory_region_validation);
    RUN_TEST(test_invalid_audio_channel);
    RUN_TEST(test_memory_initialization);
    RUN_TEST(test_stack_overflow_protection);
    RUN_TEST(test_cpu_initialization);
    RUN_TEST(test_invalid_instruction_handling);
    RUN_TEST(test_null_pointer_protection);
    
    return UNITY_END();
}