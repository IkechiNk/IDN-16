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

#define RAM_START 0x8000  
#define RAM_END 0xCFFF                    // 20KB general RAM (consolidated with former stack)

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
#define SPRITE_TABLE_END 0xD5AF           // 64 sprites x 3 bytes = 192 bytes
#define TILESET_DATA_START 0xD5B0
#define TILESET_DATA_END 0xEFFF           // 105 tiles x 64 bytes each

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
#define MAX_TILES 105
#define PALETTE_SIZE 16

// Video Control Register Offsets
#define CURSOR_X_REG (VIDEO_CONTROL_START)        // Text cursor X
#define CURSOR_Y_REG (VIDEO_CONTROL_START + 2)    // Text cursor Y

// Input Register Offsets
#define INPUT_CONTROLLER1 (INPUT_REG_START + 0)
#define INPUT_CONTROLLER2 (INPUT_REG_START + 1)

// System Control Register Offsets  
#define SYSTEM_STATUS_REG (SYSTEM_CTRL_START + 0)
#define INTERRUPT_CONTROL_REG (SYSTEM_CTRL_START + 1)
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
    bool read_only;
    bool memory_mapped_io;
    const char* name;
} MemoryRegion;

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
void memory_dump(uint8_t memory[], uint16_t start_address, uint16_t length, uint16_t chunk_size);

/* 
 * System initialization functions
 */
void initialize_video_memory(uint8_t memory[]);
void initialize_default_palette(uint8_t memory[]);
void initialize_default_tileset(uint8_t memory[]);

/* 
 * System call definitions - 8x8 font system support
 */
#define SYSCALL_CLEAR_SCREEN      0xF300    // Clear entire 40x30 text screen
#define SYSCALL_PUT_CHAR          0xF301    // Put 8x8 character at cursor position
#define SYSCALL_PUT_STRING        0xF302    // Put string using 8x8 font
#define SYSCALL_SET_CURSOR        0xF303    // Set text cursor position (x, y)
#define SYSCALL_GET_CURSOR        0xF304    // Get current cursor position
#define SYSCALL_PUT_CHAR_AT       0xF305    // Put character at specific position
#define SYSCALL_SCROLL_UP         0xF306    // Scroll text screen up one line
#define SYSCALL_FILL_AREA         0xF307    // Fill rectangular area with character
#define SYSCALL_SET_TEXT_COLOR    0xF308    // Set foreground/background color
#define SYSCALL_GET_INPUT         0xF309    // Get keyboard input
#define SYSCALL_PLAY_TONE         0xF30A    // Play audio tone
#define SYSCALL_MULTIPLY          0xF30B    // 16-bit multiply operation  
#define SYSCALL_DIVIDE            0xF30C    // 16-bit divide operation
#define SYSCALL_RANDOM            0xF30D    // Generate random number
#define SYSCALL_MEMCPY            0xF30E    // Memory copy operation
#define SYSCALL_PRINT_HEX         0xF30F    // Print number in hexadecimal
#define SYSCALL_PRINT_DEC         0xF310    // Print number in decimal
#define SYSCALL_SET_SPRITE        0xF311    // Set sprite position and tile
#define SYSCALL_SET_PALETTE       0xF312    // Set palette color
#define SYSCALL_MOVE_SPRITE       0xF313    // Move sprite to new position
#define SYSCALL_SET_SPRITE_PIXEL  0xF314    // Set individual sprite pixel color
#define SYSCALL_GET_FRAME_COUNT   0xF315    // Get current frame count
#define SYSCALL_HIDE_SPRITE       0xF316    // Hide sprite by moving off screen
#define SYSCALL_GET_SPRITE_POS    0xF317    // Get sprite x,y position
#define SYSCALL_CLEAR_SPRITE_RANGE 0xF318   // Clear sprite range
#define SYSCALL_CHECK_COLLISION   0xF319    // Check sprite collision
#define SYSCALL_SHIFT_SPRITES     0xF31A    // Move multiple sprites
#define SYSCALL_COPY_SPRITE       0xF31B    // Copy sprite properties
#define SYSCALL_SET_RETURN_ADDR   0xF31C    // Set return address register to next instruction
#define SYSCALL_MOVE_SPRITE_RIGHT 0xF31D    // Move sprite right by N tiles
#define SYSCALL_MOVE_SPRITE_LEFT  0xF31E    // Move sprite left by N tiles
#define SYSCALL_MOVE_SPRITE_UP    0xF31F    // Move sprite up by N tiles
#define SYSCALL_MOVE_SPRITE_DOWN  0xF320    // Move sprite down by N tiles
#define SYSCALL_TIMER_START       0xF321    // Start a timer
#define SYSCALL_TIMER_QUERY       0xF322    // Query timer status and optionally stop
#define SYSCALL_SLEEP             0xF323    // Sleep for specified duration

#endif // IDN16_MEMORY_H