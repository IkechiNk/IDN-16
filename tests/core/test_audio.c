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

void test_audio_register_layout(void) {
    // Test that audio register constants are correctly defined
    TEST_ASSERT_EQUAL_UINT16(0xF000, AUDIO_CH0_FREQ);
    TEST_ASSERT_EQUAL_UINT16(0xF002, AUDIO_CH0_DURATION);
    TEST_ASSERT_EQUAL_UINT16(0xF004, AUDIO_CH0_VOLUME);
    TEST_ASSERT_EQUAL_UINT16(0xF005, AUDIO_CH0_ENABLE);
    
    TEST_ASSERT_EQUAL_UINT16(0xF006, AUDIO_CH1_FREQ);
    TEST_ASSERT_EQUAL_UINT16(0xF008, AUDIO_CH1_DURATION);
    TEST_ASSERT_EQUAL_UINT16(0xF00A, AUDIO_CH1_VOLUME);
    TEST_ASSERT_EQUAL_UINT16(0xF00B, AUDIO_CH1_ENABLE);
    
    TEST_ASSERT_EQUAL_UINT16(0xF00C, AUDIO_CH2_FREQ);
    TEST_ASSERT_EQUAL_UINT16(0xF00E, AUDIO_CH2_DURATION);
    TEST_ASSERT_EQUAL_UINT16(0xF010, AUDIO_CH2_VOLUME);
    TEST_ASSERT_EQUAL_UINT16(0xF011, AUDIO_CH2_ENABLE);
    
    TEST_ASSERT_EQUAL_UINT16(0xF012, AUDIO_CH3_FREQ);
    TEST_ASSERT_EQUAL_UINT16(0xF014, AUDIO_CH3_DURATION);
    TEST_ASSERT_EQUAL_UINT16(0xF016, AUDIO_CH3_VOLUME);
    TEST_ASSERT_EQUAL_UINT16(0xF017, AUDIO_CH3_ENABLE);
    
    TEST_ASSERT_EQUAL_UINT16(0xF018, AUDIO_MASTER_VOLUME);
    TEST_ASSERT_EQUAL_UINT16(0xF019, AUDIO_GLOBAL_ENABLE);
}

void test_audio_channel_write_read(void) {
    // Test writing to channel 0
    memory_write_word(cpu->memory, AUDIO_CH0_FREQ, 880, true);
    memory_write_word(cpu->memory, AUDIO_CH0_DURATION, 2000, true);
    memory_write_byte(cpu->memory, AUDIO_CH0_VOLUME, 255, true);
    memory_write_byte(cpu->memory, AUDIO_CH0_ENABLE, 1, true);
    
    // Test reading back
    TEST_ASSERT_EQUAL_UINT16(880, memory_read_word(cpu->memory, AUDIO_CH0_FREQ));
    TEST_ASSERT_EQUAL_UINT16(2000, memory_read_word(cpu->memory, AUDIO_CH0_DURATION));
    TEST_ASSERT_EQUAL_UINT8(255, memory_read_byte(cpu->memory, AUDIO_CH0_VOLUME));
    TEST_ASSERT_EQUAL_UINT8(1, memory_read_byte(cpu->memory, AUDIO_CH0_ENABLE));
}

void test_audio_channel_isolation(void) {
    // Set different values for each channel
    memory_write_word(cpu->memory, AUDIO_CH0_FREQ, 440, true);
    memory_write_word(cpu->memory, AUDIO_CH1_FREQ, 880, true);
    memory_write_word(cpu->memory, AUDIO_CH2_FREQ, 1320, true);
    memory_write_word(cpu->memory, AUDIO_CH3_FREQ, 1760, true);
    
    // Verify each channel maintains its own value
    TEST_ASSERT_EQUAL_UINT16(440, memory_read_word(cpu->memory, AUDIO_CH0_FREQ));
    TEST_ASSERT_EQUAL_UINT16(880, memory_read_word(cpu->memory, AUDIO_CH1_FREQ));
    TEST_ASSERT_EQUAL_UINT16(1320, memory_read_word(cpu->memory, AUDIO_CH2_FREQ));
    TEST_ASSERT_EQUAL_UINT16(1760, memory_read_word(cpu->memory, AUDIO_CH3_FREQ));
}

void test_audio_master_controls(void) {
    // Test master volume
    memory_write_byte(cpu->memory, AUDIO_MASTER_VOLUME, 127, true);
    TEST_ASSERT_EQUAL_UINT8(127, memory_read_byte(cpu->memory, AUDIO_MASTER_VOLUME));
    
    // Test global enable
    memory_write_byte(cpu->memory, AUDIO_GLOBAL_ENABLE, 1, true);
    TEST_ASSERT_EQUAL_UINT8(1, memory_read_byte(cpu->memory, AUDIO_GLOBAL_ENABLE));
    
    memory_write_byte(cpu->memory, AUDIO_GLOBAL_ENABLE, 0, true);
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_GLOBAL_ENABLE));
}

void test_audio_syscall_integration(void) {
    // Test integrated syscall functionality
    
    // Play tone on channel 2
    cpu->r[1] = 2;      // Channel 2
    cpu->r[2] = 523;    // C5 frequency
    cpu->r[3] = 500;    // 500ms duration
    cpu->r[4] = 180;    // Volume 180
    
    syscall_play_tone_channel(cpu);
    
    // Verify syscall succeeded
    TEST_ASSERT_EQUAL_UINT16(1, cpu->r[1]);
    
    // Verify registers were set correctly
    TEST_ASSERT_EQUAL_UINT16(523, memory_read_word(cpu->memory, AUDIO_CH2_FREQ));
    TEST_ASSERT_EQUAL_UINT16(500, memory_read_word(cpu->memory, AUDIO_CH2_DURATION));
    TEST_ASSERT_EQUAL_UINT8(180, memory_read_byte(cpu->memory, AUDIO_CH2_VOLUME));
    TEST_ASSERT_EQUAL_UINT8(1, memory_read_byte(cpu->memory, AUDIO_CH2_ENABLE));
    
    // Stop the channel
    cpu->r[1] = 2;
    syscall_stop_channel(cpu);
    
    // Verify channel is disabled
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH2_ENABLE));
}

void test_audio_volume_bounds(void) {
    // Test volume bounds (0-255)
    memory_write_byte(cpu->memory, AUDIO_CH0_VOLUME, 0, true);
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_CH0_VOLUME));
    
    memory_write_byte(cpu->memory, AUDIO_CH0_VOLUME, 255, true);
    TEST_ASSERT_EQUAL_UINT8(255, memory_read_byte(cpu->memory, AUDIO_CH0_VOLUME));
    
    memory_write_byte(cpu->memory, AUDIO_MASTER_VOLUME, 0, true);
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(cpu->memory, AUDIO_MASTER_VOLUME));
    
    memory_write_byte(cpu->memory, AUDIO_MASTER_VOLUME, 255, true);
    TEST_ASSERT_EQUAL_UINT8(255, memory_read_byte(cpu->memory, AUDIO_MASTER_VOLUME));
}

void test_audio_frequency_range(void) {
    // Test frequency range (0-65535 Hz)
    memory_write_word(cpu->memory, AUDIO_CH1_FREQ, 0, true);
    TEST_ASSERT_EQUAL_UINT16(0, memory_read_word(cpu->memory, AUDIO_CH1_FREQ));
    
    memory_write_word(cpu->memory, AUDIO_CH1_FREQ, 65535, true);
    TEST_ASSERT_EQUAL_UINT16(65535, memory_read_word(cpu->memory, AUDIO_CH1_FREQ));
    
    // Test common musical frequencies
    memory_write_word(cpu->memory, AUDIO_CH1_FREQ, 440, true);    // A4
    TEST_ASSERT_EQUAL_UINT16(440, memory_read_word(cpu->memory, AUDIO_CH1_FREQ));
    
    memory_write_word(cpu->memory, AUDIO_CH1_FREQ, 880, true);    // A5
    TEST_ASSERT_EQUAL_UINT16(880, memory_read_word(cpu->memory, AUDIO_CH1_FREQ));
}

void test_audio_duration_range(void) {
    // Test duration range (0-65535 ms)
    memory_write_word(cpu->memory, AUDIO_CH3_DURATION, 0, true);
    TEST_ASSERT_EQUAL_UINT16(0, memory_read_word(cpu->memory, AUDIO_CH3_DURATION));
    
    memory_write_word(cpu->memory, AUDIO_CH3_DURATION, 65535, true);
    TEST_ASSERT_EQUAL_UINT16(65535, memory_read_word(cpu->memory, AUDIO_CH3_DURATION));
    
    // Test common durations
    memory_write_word(cpu->memory, AUDIO_CH3_DURATION, 1000, true);  // 1 second
    TEST_ASSERT_EQUAL_UINT16(1000, memory_read_word(cpu->memory, AUDIO_CH3_DURATION));
    
    memory_write_word(cpu->memory, AUDIO_CH3_DURATION, 500, true);   // 0.5 seconds
    TEST_ASSERT_EQUAL_UINT16(500, memory_read_word(cpu->memory, AUDIO_CH3_DURATION));
}

int main(void) {
    UNITY_BEGIN();
    
    RUN_TEST(test_audio_register_layout);
    RUN_TEST(test_audio_channel_write_read);
    RUN_TEST(test_audio_channel_isolation);
    RUN_TEST(test_audio_master_controls);
    RUN_TEST(test_audio_syscall_integration);
    RUN_TEST(test_audio_volume_bounds);
    RUN_TEST(test_audio_frequency_range);
    RUN_TEST(test_audio_duration_range);
    
    return UNITY_END();
}