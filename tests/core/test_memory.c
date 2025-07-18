#include "../unity/unity.h"
#include "idn16/memory.h"
#include <SDL3/SDL.h>

static uint8_t test_memory[MEMORY_SIZE];

void setUp(void) {
    memory_init(test_memory);
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
}

void tearDown(void) {
    SDL_Quit();
}

void test_initialize_default_palette_sets_default_palette(void) {
    initialize_default_palette(test_memory);
    // Compare memory values to palette values
    for (int i = 0; i < PALETTE_SIZE; i++)
    {
        TEST_ASSERT_EQUAL_HEX8(default_colors[i], test_memory[PALETTE_RAM_START + (i * 2)]);
    }    
}

void test_memory_write_and_read_byte(void) {
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, RAM_START, 0xAB, false));
    TEST_ASSERT_EQUAL_UINT8(0xAB, memory_read_byte(test_memory, RAM_START));
    
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, RAM_END - 1, 0xCD, false));
    TEST_ASSERT_EQUAL_UINT8(0xCD, memory_read_byte(test_memory, RAM_END - 1));
}

void test_memory_write_and_read_word(void) {
    TEST_ASSERT_TRUE(memory_write_word(test_memory, RAM_START, 0xBEEF, false));
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, memory_read_word(test_memory, RAM_START));
    
    TEST_ASSERT_TRUE(memory_write_word(test_memory, VIDEO_RAM_END - 2, 0xBEEF, false));
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, memory_read_word(test_memory, VIDEO_RAM_END - 2));
    
    TEST_ASSERT_FALSE(memory_write_word(test_memory, VIDEO_RAM_END, 0xBEEF, false));
}

void test_memory_basic_operations(void) {
    // Test reading from the last valid byte (should return 0)
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(test_memory, MEMORY_SIZE - 1));
    
    // Test writing a byte to a valid RAM address (should succeed)
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, RAM_START + 100, 0x42, false));
    TEST_ASSERT_EQUAL_UINT8(0x42, memory_read_byte(test_memory, RAM_START + 100));
    
    // Test writing a word to a valid RAM address
    TEST_ASSERT_TRUE(memory_write_word(test_memory, RAM_START + 200, 0x1234, false));
    TEST_ASSERT_EQUAL_HEX16(0x1234, memory_read_word(test_memory, RAM_START + 200));
}

void test_memory_get_region(void) {
    TEST_ASSERT_EQUAL(REGION_USER_ROM, memory_get_region(USER_ROM_START));
    TEST_ASSERT_EQUAL(REGION_RAM, memory_get_region(RAM_START));
    TEST_ASSERT_EQUAL(REGION_VIDEO, memory_get_region(VIDEO_RAM_START));
    TEST_ASSERT_EQUAL(REGION_SYSCALL, memory_get_region(MEMORY_SIZE - 1));
}

void test_memory_dump_runs(void) {
    // Just check that it doesn't crash
    TEST_ASSERT_TRUE(memory_write_word(test_memory, 0x3333, 0xA123, true));
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, 0x3335, 0xCC, true));
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, 0x3336, 0xDD, true));
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, 0x3337, 0xEE, true));
    memory_dump(test_memory, 0x3333, 2, 4);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize_default_palette_sets_default_palette);
    RUN_TEST(test_memory_write_and_read_byte);
    RUN_TEST(test_memory_write_and_read_word);
    RUN_TEST(test_memory_basic_operations);
    RUN_TEST(test_memory_get_region);
    RUN_TEST(test_memory_dump_runs);
    return UNITY_END();
}