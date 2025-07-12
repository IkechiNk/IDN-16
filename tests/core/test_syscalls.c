#include "../unity/unity.h"
#include "idn16/cpu.h"
#include "idn16/memory.h"
#include <string.h>

static Cpu_t* cpu;

void setUp(void) {
    cpu = cpu_init();
    TEST_ASSERT_NOT_NULL(cpu);
}

void tearDown(void) {
    cpu_destroy(cpu);
}

void test_syscall_clear_screen(void) {
    // Fill screen with non-space characters
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        memory_write_byte(cpu->memory, CHAR_BUFFER_START + i, 'X', true);
    }
    
    // Set cursor to non-zero position
    memory_write_word(cpu->memory, CURSOR_X_REG, 10, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, 5, true);
    
    // Call clear screen syscall
    syscall_clear_screen(cpu);
    
    // Check that screen is cleared (filled with spaces)
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        uint8_t ch = memory_read_byte(cpu->memory, CHAR_BUFFER_START + i);
        TEST_ASSERT_EQUAL_UINT8(32, ch); // 32 = space character
    }
    
    // Check cursor is reset to 0,0
    TEST_ASSERT_EQUAL_UINT16(0, memory_read_word(cpu->memory, CURSOR_X_REG));
    TEST_ASSERT_EQUAL_UINT16(0, memory_read_word(cpu->memory, CURSOR_Y_REG));
    
    // Check success returned
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
}

void test_syscall_put_char(void) {
    // Set cursor to known position
    memory_write_word(cpu->memory, CURSOR_X_REG, 5, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, 3, true);
    
    // Put character 'A'
    cpu->r[1] = 'A';
    syscall_put_char(cpu);
    
    // Check character was written
    uint16_t tile_addr = CHAR_BUFFER_START + (3 * SCREEN_WIDTH_TILES) + 5;
    uint8_t ch = memory_read_byte(cpu->memory, tile_addr);
    TEST_ASSERT_EQUAL_UINT8('A', ch);
    
    // Check cursor advanced
    TEST_ASSERT_EQUAL_UINT16(6, memory_read_word(cpu->memory, CURSOR_X_REG));
    TEST_ASSERT_EQUAL_UINT16(3, memory_read_word(cpu->memory, CURSOR_Y_REG));
}

void test_syscall_put_char_newline(void) {
    // Set cursor to known position
    memory_write_word(cpu->memory, CURSOR_X_REG, 10, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, 5, true);
    
    // Put newline character
    cpu->r[1] = '\n';
    syscall_put_char(cpu);
    
    // Check cursor moved to next line
    TEST_ASSERT_EQUAL_UINT16(0, memory_read_word(cpu->memory, CURSOR_X_REG));
    TEST_ASSERT_EQUAL_UINT16(6, memory_read_word(cpu->memory, CURSOR_Y_REG));
}

void test_syscall_play_tone_channel(void) {
    // Test channel 0
    cpu->r[1] = 0;      // Channel 0
    cpu->r[2] = 440;    // 440Hz frequency
    cpu->r[3] = 1000;   // 1000ms duration
    cpu->r[4] = 200;    // Volume 200
    
    syscall_play_tone_channel(cpu);
    
    // Check success
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
    
    // Check audio registers were written
    TEST_ASSERT_EQUAL_UINT16(440, memory_read_word(cpu->memory, AUDIO_CH0_FREQ));
    TEST_ASSERT_EQUAL_UINT16(1000, memory_read_word(cpu->memory, AUDIO_CH0_DURATION));
    TEST_ASSERT_EQUAL_UINT8(200, memory_read_byte(cpu->memory, AUDIO_CH0_VOLUME));
    TEST_ASSERT_EQUAL_UINT8(1, memory_read_byte(cpu->memory, AUDIO_CH0_ENABLE));
}

void test_syscall_play_tone_channel_invalid(void) {
    // Test invalid channel
    cpu->r[1] = 5;      // Invalid channel (only 0-3 valid)
    cpu->r[2] = 440;    // 440Hz frequency
    cpu->r[3] = 1000;   // 1000ms duration
    cpu->r[4] = 200;    // Volume 200
    
    syscall_play_tone_channel(cpu);
    
    // Check failure
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
}

void test_syscall_stop_channel(void) {
    // Enable channel 1 first
    memory_write_byte(cpu->memory, AUDIO_CH1_ENABLE, 1, true);
    
    // Stop channel 1
    cpu->r[1] = 1;
    syscall_stop_channel(cpu);
    
    // Check success
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
    
    // Check channel is disabled
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH1_ENABLE));
}

void test_syscall_stop_channel_invalid(void) {
    // Test invalid channel
    cpu->r[1] = 4;      // Invalid channel
    syscall_stop_channel(cpu);
    
    // Check failure
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
}

void test_syscall_set_master_volume(void) {
    cpu->r[1] = 150;    // Volume 150
    syscall_set_master_volume(cpu);
    
    // Check success
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
    
    // Check master volume was set
    TEST_ASSERT_EQUAL_UINT8(150, memory_read_byte(cpu->memory, AUDIO_MASTER_VOLUME));
}

void test_syscall_stop_all_audio(void) {
    // Enable all channels first
    memory_write_byte(cpu->memory, AUDIO_CH0_ENABLE, 1, true);
    memory_write_byte(cpu->memory, AUDIO_CH1_ENABLE, 1, true);
    memory_write_byte(cpu->memory, AUDIO_CH2_ENABLE, 1, true);
    memory_write_byte(cpu->memory, AUDIO_CH3_ENABLE, 1, true);
    memory_write_byte(cpu->memory, AUDIO_GLOBAL_ENABLE, 1, true);
    
    syscall_stop_all_audio(cpu);
    
    // Check success
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
    
    // Check all channels are disabled
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH0_ENABLE));
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH1_ENABLE));
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH2_ENABLE));
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH3_ENABLE));
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_GLOBAL_ENABLE));
}

void test_syscall_multiply(void) {
    cpu->r[1] = 12;
    cpu->r[2] = 34;
    
    syscall_multiply(cpu);
    
    // 12 * 34 = 408 (0x0198)
    TEST_ASSERT_EQUAL_UINT16(408, cpu->r[1]);  // Lower 16 bits
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[2]);    // Upper 16 bits
}

void test_syscall_divide(void) {
    cpu->r[1] = 100;
    cpu->r[2] = 7;
    
    syscall_divide(cpu);
    
    // 100 / 7 = 14 remainder 2
    TEST_ASSERT_EQUAL_UINT16(14, cpu->r[1]);   // Quotient
    TEST_ASSERT_EQUAL_UINT16(2, cpu->r[2]);    // Remainder
}

void test_syscall_divide_by_zero(void) {
    cpu->r[1] = 100;
    cpu->r[2] = 0;
    
    syscall_divide(cpu);
    
    // Division by zero should return 0
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[1]);
    TEST_ASSERT_EQUAL_UINT16(0, cpu->r[2]);
}

void test_syscall_get_input(void) {
    // Set controller input
    memory_write_byte(cpu->memory, INPUT_CONTROLLER1, 0x55, true);
    
    syscall_get_input(cpu);
    
    // Check input was read
    TEST_ASSERT_EQUAL_UINT16(0x55, cpu->r[1]);
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_syscall_clear_screen);
    RUN_TEST(test_syscall_put_char);
    RUN_TEST(test_syscall_put_char_newline);
    RUN_TEST(test_syscall_play_tone_channel);
    RUN_TEST(test_syscall_play_tone_channel_invalid);
    RUN_TEST(test_syscall_stop_channel);
    RUN_TEST(test_syscall_stop_channel_invalid);
    RUN_TEST(test_syscall_set_master_volume);
    RUN_TEST(test_syscall_stop_all_audio);
    RUN_TEST(test_syscall_multiply);
    RUN_TEST(test_syscall_divide);
    RUN_TEST(test_syscall_divide_by_zero);
    RUN_TEST(test_syscall_get_input);
    
    return UNITY_END();
}