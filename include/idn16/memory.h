#ifndef IDN16_MEMORY_H
#define IDN16_MEMORY_H

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Memory region sizes
#define MEMORY_SIZE 65536 // 64KB

// Memory Map addressing constraints
#define USER_ROM_START 0x0000
#define USER_ROM_END 0x7FFF               // 32KB user program ROM

#define RAM_START 0x8000  
#define RAM_END 0xCFFF                    // 20KB general RAM (and by extension stack)

// Video Memory Layout
#define VIDEO_RAM_START 0xD000
#define VIDEO_RAM_END 0xEFFF              // 8KB video RAM

#define CHAR_BUFFER_START 0xD000
#define CHAR_BUFFER_END 0xD4AF            // 40x30 chars = 1200 bytes
#define VIDEO_CONTROL_START 0xD4B0
#define VIDEO_CONTROL_END 0xD4CF          // Video control registers = 32 bytes
#define PALETTE_RAM_START 0xD4D0
#define PALETTE_RAM_END 0xD4EF            // 16 colors x 2 bytes = 32 bytes
#define SPRITE_TABLE_START 0xD4F0  
#define SPRITE_TABLE_END 0xE2FF           // 1200 sprites x 3 bytes = 3600 bytes
#define TILESET_DATA_START 0xE300
#define TILESET_DATA_END 0xEFFF           // 51 tiles x 64 bytes each

// I/O and System Control - 4KB total
#define AUDIO_REG_START 0xF000
#define AUDIO_REG_END 0xF0FF              // 256 bytes audio
#define INPUT_REG_START 0xF100
#define INPUT_REG_END 0xF1FF              // 256 bytes input
#define SYSTEM_CTRL_START 0xF200
#define SYSTEM_CTRL_END 0xF2FF            // 256 bytes system control

// System call addresses (trap to C functions)
#define SYSCALL_BASE 0xF300               // System calls start here
#define SYSCALL_END 0xFFFF                // System call range

// Display constants
#define SCREEN_WIDTH_TILES 40
#define SCREEN_HEIGHT_TILES 30
#define TILE_SIZE 8
#define SCREEN_WIDTH_PIXELS (SCREEN_WIDTH_TILES * TILE_SIZE)
#define SCREEN_HEIGHT_PIXELS (SCREEN_HEIGHT_TILES * TILE_SIZE)
#define MAX_SPRITES 1200
#define MAX_TILES 51
#define PALETTE_SIZE 16

// Video Control Register Offsets
#define CURSOR_X_REG (VIDEO_CONTROL_START)        // Text cursor X
#define CURSOR_Y_REG (VIDEO_CONTROL_START + 2)    // Text cursor Y

// Input Register Offsets
#define INPUT_CONTROLLER1 (INPUT_REG_START + 0)
#define INPUT_CONTROLLER2 (INPUT_REG_START + 1)

// Audio Channel Register Offsets
#define AUDIO_CH0_FREQ (AUDIO_REG_START + 0x00)
#define AUDIO_CH0_DURATION (AUDIO_REG_START + 0x02)
#define AUDIO_CH0_VOLUME (AUDIO_REG_START + 0x04)
#define AUDIO_CH0_ENABLE (AUDIO_REG_START + 0x05)

#define AUDIO_CH1_FREQ (AUDIO_REG_START + 0x06)
#define AUDIO_CH1_DURATION (AUDIO_REG_START + 0x08)
#define AUDIO_CH1_VOLUME (AUDIO_REG_START + 0x0A)
#define AUDIO_CH1_ENABLE (AUDIO_REG_START + 0x0B)

#define AUDIO_CH2_FREQ (AUDIO_REG_START + 0x0C)
#define AUDIO_CH2_DURATION (AUDIO_REG_START + 0x0E)
#define AUDIO_CH2_VOLUME (AUDIO_REG_START + 0x10)
#define AUDIO_CH2_ENABLE (AUDIO_REG_START + 0x11)

#define AUDIO_CH3_FREQ (AUDIO_REG_START + 0x12)
#define AUDIO_CH3_DURATION (AUDIO_REG_START + 0x14)
#define AUDIO_CH3_VOLUME (AUDIO_REG_START + 0x16)
#define AUDIO_CH3_ENABLE (AUDIO_REG_START + 0x17)

#define AUDIO_MASTER_VOLUME (AUDIO_REG_START + 0x18)
#define AUDIO_GLOBAL_ENABLE (AUDIO_REG_START + 0x19)

// System Control Register Offsets  
#define TIMER_COUNTER_LOW (SYSTEM_CTRL_START + 2)
#define TIMER_COUNTER_HIGH (SYSTEM_CTRL_START + 3)
#define FRAME_COUNTER_LOW (SYSTEM_CTRL_START + 6)
#define FRAME_COUNTER_HIGH (SYSTEM_CTRL_START + 8)

// Memory regions
typedef enum {
    REGION_USER_ROM,
    REGION_RAM, 
    REGION_VIDEO,
    REGION_AUDIO,
    REGION_INPUT,
    REGION_SYSTEM_CTRL,
    REGION_SYSCALL,
    REGION_COUNT
} MemoryRegion_t;

// Memory region structure
typedef struct {
    uint16_t start_address;
    uint16_t end_address;
    bool privileged;
    const char* name;
} MemoryRegion;

// 16-color palette (RGB565 format)
extern const uint16_t default_colors[16];

/* 
 * Initializes memory with default values
 */
void memory_init(uint8_t memory[]); 

bool load_user_rom(uint8_t memory[], FILE* rom);

/* 
 * Memory access functions
 */
uint8_t memory_read_byte(uint8_t memory[], uint16_t address);
uint16_t memory_read_word(uint8_t memory[], uint16_t address);
bool memory_write_byte(uint8_t memory[], uint16_t address, uint8_t data, bool privileged);
bool memory_write_word(uint8_t memory[], uint16_t address, uint16_t data, bool privileged);

/* 
 * Memory region management
 */
MemoryRegion_t memory_get_region(uint16_t address);
void memory_dump(uint8_t memory[], uint16_t start_addr, uint16_t bytes_per_line, uint16_t num_lines);

/* 
 * System initialization functions
 */
void initialize_video_memory(uint8_t memory[]);
void initialize_default_palette(uint8_t memory[]);
void initialize_default_tileset(uint8_t memory[]);
void initialize_audio_system(uint8_t memory[]);

/* 
 * System call definitions - 8x8 font system support
 */
#define SYSCALL_CLEAR_SCREEN        0xF300
#define SYSCALL_PUT_CHAR            0xF301
#define SYSCALL_PUT_STRING          0xF302
#define SYSCALL_SET_CURSOR          0xF303
#define SYSCALL_GET_CURSOR          0xF304
#define SYSCALL_PUT_CHAR_AT         0xF305
#define SYSCALL_SCROLL_UP           0xF306
#define SYSCALL_FILL_AREA           0xF307
#define SYSCALL_SET_TEXT_COLOR      0xF308
#define SYSCALL_GET_INPUT           0xF309
#define SYSCALL_PLAY_TONE_CHANNEL   0xF30A
#define SYSCALL_MULTIPLY            0xF30B
#define SYSCALL_DIVIDE              0xF30C
#define SYSCALL_RANDOM              0xF30D
#define SYSCALL_MEMCPY              0xF30E
#define SYSCALL_PRINT_HEX           0xF30F
#define SYSCALL_PRINT_DEC           0xF310
#define SYSCALL_SET_SPRITE          0xF311
#define SYSCALL_SET_PALETTE         0xF312
#define SYSCALL_MOVE_SPRITE         0xF313
#define SYSCALL_SET_TILE_PIXEL      0xF314
#define SYSCALL_GET_FRAME_COUNT     0xF315
#define SYSCALL_HIDE_SPRITE         0xF316
#define SYSCALL_GET_SPRITE_POS      0xF317
#define SYSCALL_CLEAR_SPRITE_RANGE  0xF318
#define SYSCALL_CHECK_COLLISION     0xF319
#define SYSCALL_SHIFT_SPRITES       0xF31A
#define SYSCALL_COPY_SPRITE         0xF31B
#define SYSCALL_SET_RETURN_ADDR     0xF31C
#define SYSCALL_MOVE_SPRITE_RIGHT   0xF31D
#define SYSCALL_MOVE_SPRITE_LEFT    0xF31E
#define SYSCALL_MOVE_SPRITE_UP      0xF31F
#define SYSCALL_MOVE_SPRITE_DOWN    0xF320
#define SYSCALL_TIMER_START         0xF321
#define SYSCALL_TIMER_QUERY         0xF322
#define SYSCALL_SLEEP               0xF323
#define SYSCALL_NUMBER_TO_STRING    0xF324
#define SYSCALL_STOP_CHANNEL        0xF325
#define SYSCALL_SET_MASTER_VOLUME   0xF326
#define SYSCALL_STOP_ALL_AUDIO      0xF327

#endif // IDN16_MEMORY_H