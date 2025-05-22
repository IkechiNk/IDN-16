#include "Unity/unity.h"
#include "../core/memory.h"
#include <SDL2/SDL.h>

static uint8_t test_memory[MEMORY_SIZE];

void setUp(void) {
    memory_init(test_memory);
    SDL_Init(SDL_INIT_EVERYTHING);
}

void tearDown(void) {
    SDL_Quit();
}

void test_initialize_palettes_sets_default_palette(void) {
    initialize_palettes(test_memory);
    // Check first palette value (should be 0x0000)
    TEST_ASSERT_EQUAL_HEX8(0x00, test_memory[PALETTE_MEM_START]);
}

void test_memory_write_and_read_byte(void) {
    uint16_t addr = RAM_START + 10;
    TEST_ASSERT_TRUE(memory_write_byte(test_memory, addr, 0xAB));
    TEST_ASSERT_EQUAL_UINT8(0xAB, memory_read_byte(test_memory, addr));
}

void test_memory_write_and_read_word(void) {
    uint16_t addr = RAM_START + 20;
    TEST_ASSERT_TRUE(memory_write_word(test_memory, addr, 0xBEEF));
    uint16_t val = memory_read_word(test_memory, addr);
    memory_dump(test_memory, addr, 4, 4);
    TEST_ASSERT_EQUAL_HEX16(0xBEEF, val);
}

void test_memory_write_read_only(void) {
    // ROM is read-only
    TEST_ASSERT_FALSE(memory_write_byte(test_memory, ROM_START, 0x12));
    TEST_ASSERT_FALSE(memory_write_word(test_memory, ROM_START, 0x1234));
}

void test_memory_out_of_bounds(void) {
    TEST_ASSERT_FALSE(memory_write_byte(test_memory, 0x1FFF, 0x12));
    TEST_ASSERT_FALSE(memory_write_word(test_memory, MEMORY_SIZE - 1, 0x12));
    TEST_ASSERT_EQUAL_UINT8(0, memory_read_byte(test_memory, MEMORY_SIZE - 1));
}

void test_memory_get_region(void) {
    TEST_ASSERT_EQUAL(REGION_ROM, memory_get_region(ROM_START));
    TEST_ASSERT_EQUAL(REGION_RAM, memory_get_region(RAM_START));
    TEST_ASSERT_EQUAL(REGION_VIDEO, memory_get_region(VIDEO_MEM_START));
    TEST_ASSERT_EQUAL(REGION_COUNT, memory_get_region(MEMORY_SIZE - 1));
}

void test_handle_input_read_default(void) {
    // Should return 0 for invalid offset
    TEST_ASSERT_EQUAL_UINT8(0, handle_input_read(0xFF));
}

void test_handle_sound_read_invalid_channel(void) {
    TEST_ASSERT_EQUAL_UINT8(0, handle_sound_read(0x100));
}

void test_handle_input_write_readonly(void) {
    // Should print warning and return false for read-only
    TEST_ASSERT_FALSE(handle_input_write(0x00, 0x12));
    TEST_ASSERT_FALSE(handle_input_write(0x01, 0x34));
    // Should return true for other offsets
    TEST_ASSERT_TRUE(handle_input_write(0x10, 0x34));
}

void test_handle_sound_write_cases(void) {
    // Should return true for valid register
    TEST_ASSERT_TRUE(handle_sound_write(0x00, 0x56));
    TEST_ASSERT_TRUE(handle_sound_write(0x01, 0x78));
    TEST_ASSERT_TRUE(handle_sound_write(0x02, 0x9A));
    // Should return true for default case
    TEST_ASSERT_TRUE(handle_sound_write(0x0F, 0xBC));
}

void test_memory_dump_runs(void) {
    // Just check that it doesn't crash
    memory_write_word(test_memory, 0x3333, 0xA123);
    memory_write_byte(test_memory, 0x3335, 0xCC);
    memory_write_byte(test_memory, 0x3336, 0xDD);
    memory_write_byte(test_memory, 0x3337, 0xEE);
    memory_dump(test_memory, 0x3333, 2, 4);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_initialize_palettes_sets_default_palette);
    RUN_TEST(test_memory_write_and_read_byte);
    RUN_TEST(test_memory_write_and_read_word);
    RUN_TEST(test_memory_write_read_only);
    RUN_TEST(test_memory_out_of_bounds);
    RUN_TEST(test_memory_get_region);
    RUN_TEST(test_handle_input_read_default);
    RUN_TEST(test_handle_sound_read_invalid_channel);
    RUN_TEST(test_handle_input_write_readonly);
    RUN_TEST(test_handle_sound_write_cases);
    RUN_TEST(test_memory_dump_runs);
    return UNITY_END();
}