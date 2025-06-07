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
#define ROM_START 0x0000
#define ROM_END 0x1FFF                    // 8KB user ROM
#define RAM_START 0x2000  
#define RAM_END 0x2FFF                    // 4KB general RAM

// Video Memory Layout
#define VIDEO_RAM_START 0x3000
#define VIDEO_RAM_END 0x3FFF              // 4KB video RAM

#define TILE_BUFFER_START 0x3000
#define TILE_BUFFER_END 0x34FF            // 40x30 tiles = 1200 bytes
#define SPRITE_TABLE_START 0x3500
#define SPRITE_TABLE_END 0x35FF           // 64 sprites x 4 bytes = 256 bytes
#define PALETTE_RAM_START 0x3600
#define PALETTE_RAM_END 0x367F            // 64 colors x 2 bytes = 128 bytes
#define VIDEO_CONTROL_START 0x3680
#define VIDEO_CONTROL_END 0x369F          // Video control registers = 32 bytes
#define TILESET_DATA_START 0x36A0
#define TILESET_DATA_END 0x3FFF           // 256 tiles x 32 bytes each

// Audio, Input, System
#define AUDIO_REG_START 0x4000
#define AUDIO_REG_END 0x40FF              // 256 bytes audio
#define INPUT_REG_START 0x4100
#define INPUT_REG_END 0x41FF              // 256 bytes input
#define SYSTEM_CTRL_START 0x4200
#define SYSTEM_CTRL_END 0x42FF            // 256 bytes system control

// System ROM - massive space for built-in functions
#define SYSTEM_ROM_START 0x4300
#define SYSTEM_ROM_END 0xFFFF             // ~47KB system ROM

// Display constants
#define SCREEN_WIDTH_TILES 40
#define SCREEN_HEIGHT_TILES 30
#define TILE_SIZE 8
#define SCREEN_WIDTH_PIXELS (SCREEN_WIDTH_TILES * TILE_SIZE)
#define SCREEN_HEIGHT_PIXELS (SCREEN_HEIGHT_TILES * TILE_SIZE)
#define MAX_SPRITES 64
#define MAX_TILES 256
#define PALETTE_SIZE 64

// Video Control Register Offsets
#define VIDEO_MODE_REG (VIDEO_CONTROL_START + 0)      // Current video mode
#define SCROLL_X_REG (VIDEO_CONTROL_START + 2)        // Background scroll X
#define SCROLL_Y_REG (VIDEO_CONTROL_START + 4)        // Background scroll Y
#define CURSOR_X_REG (VIDEO_CONTROL_START + 6)        // Text cursor X
#define CURSOR_Y_REG (VIDEO_CONTROL_START + 8)        // Text cursor Y

// Video Modes
#define VIDEO_MODE_TEXT 0
#define VIDEO_MODE_TILES 1  
#define VIDEO_MODE_MIXED 2

// Input Register Offsets
#define INPUT_CONTROLLER1 (INPUT_REG_START + 0)
#define INPUT_CONTROLLER2 (INPUT_REG_START + 1)

// System Control Register Offsets  
#define SYSTEM_STATUS_REG (SYSTEM_CTRL_START + 0)
#define INTERRUPT_CONTROL_REG (SYSTEM_CTRL_START + 1)
#define TIMER_COUNTER_LOW (SYSTEM_CTRL_START + 2)
#define TIMER_COUNTER_HIGH (SYSTEM_CTRL_START + 4)
#define FRAME_COUNTER_LOW (SYSTEM_CTRL_START + 6)
#define FRAME_COUNTER_HIGH (SYSTEM_CTRL_START + 8)

// Memory regions
typedef enum {
    REGION_ROM,
    REGION_RAM, 
    REGION_VIDEO,
    REGION_AUDIO,
    REGION_INPUT,
    REGION_SYSTEM_CTRL,
    REGION_SYSTEM_ROM,
    REGION_COUNT
} MemoryRegion_t;

// Memory region structure
typedef struct {
    uint16_t start_address;
    uint16_t end_address;
    bool read_only;
    bool memory_mapped_io;
    const char* name;
} MemoryRegion;

/* 
 * Initializes memory with default values and system ROM
 */
void memory_init(uint8_t memory[]); 

bool load_rom(uint8_t memory[], const char* rom);

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
void memory_dump(uint8_t memory[], uint16_t start_address, uint16_t length, uint16_t chunk_size);

/* 
 * System initialization functions
 */
void initialize_video_memory(uint8_t memory[]);
void initialize_default_palette(uint8_t memory[]);
void initialize_default_tileset(uint8_t memory[]);
void load_system_rom(uint8_t memory[]);
void create_minimal_system_rom(uint8_t memory[]);

#endif // IDN16_MEMORY_H