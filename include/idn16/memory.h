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
#define USER_ROM_END 0x7FFF               // 32KB user program ROM (4x larger)

#define STACK_START 0x8000
#define STACK_END 0x8FFF                  // 4KB hardware stack

#define RAM_START 0x9000  
#define RAM_END 0xCFFF                    // 16KB general RAM (4x larger)

// Video Memory Layout
#define VIDEO_RAM_START 0xD000
#define VIDEO_RAM_END 0xEFFF              // 8KB video RAM

#define TILE_BUFFER_START 0xD000
#define TILE_BUFFER_END 0xD4FF            // 40x30 tiles = 1200 bytes
#define SPRITE_TABLE_START 0xD500  
#define SPRITE_TABLE_END 0xD5FF           // 64 sprites x 4 bytes = 256 bytes
#define PALETTE_RAM_START 0xD600
#define PALETTE_RAM_END 0xD67F            // 64 colors x 2 bytes = 128 bytes
#define VIDEO_CONTROL_START 0xD680
#define VIDEO_CONTROL_END 0xD69F          // Video control registers = 32 bytes
#define TILESET_DATA_START 0xD6A0
#define TILESET_DATA_END 0xDFFF           // 256 tiles x 32 bytes each
#define VIDEO_BUFFER_START 0xE000
#define VIDEO_BUFFER_END 0xEFFF           // 4KB frame buffer for scrolling

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
    REGION_USER_ROM,
    REGION_STACK,
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
    bool read_only;
    bool memory_mapped_io;
    const char* name;
} MemoryRegion;

/* 
 * Initializes memory with default values
 */
void memory_init(uint8_t memory[]); 

bool load_user_rom(uint8_t memory[], const char* rom);

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
void create_font_tile(uint8_t memory[], int tile_id, uint8_t bitmap[8]);

/* 
 * System call definitions
 */
#define SYSCALL_CLEAR_SCREEN    0xF300
#define SYSCALL_PUT_CHAR        0xF301
#define SYSCALL_PUT_STRING      0xF302
#define SYSCALL_GET_INPUT       0xF303
#define SYSCALL_PLAY_TONE       0xF304
#define SYSCALL_MULTIPLY        0xF305
#define SYSCALL_DIVIDE          0xF306
#define SYSCALL_RANDOM          0xF307
#define SYSCALL_MEMCPY          0xF308

#endif // IDN16_MEMORY_H