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
#define ROM_SIZE 8192 // 8KB
#define RAM_SIZE 8192 // 8KB
#define VIDEO_MEM_SIZE 2048 // 2KB
#define SPRITE_MEM_SIZE 2048 // 2KB
#define PALETTE_MEM_SIZE 256 // 256B
#define SOUND_REG_SIZE 256 // 256B
#define INPUT_REG_SIZE 256 // 256B
#define SYSTEM_CTRL_SIZE 256 // 256B
#define SYSTEM_ROM_SIZE 4096 // 4KB

// Memory Map adressing constraints
#define ROM_START 0x0000
#define ROM_END 0x1FFF
#define RAM_START 0x2000
#define RAM_END 0x3FFF
#define VIDEO_MEM_START 0x4000
#define VIDEO_MEM_END 0x47FF
#define SPRITE_MEM_START 0x4800
#define SPRITE_MEM_END 0x4FFF
#define PALETTE_MEM_START 0x5000
#define PALETTE_MEM_END 0x50FF
#define SOUND_REG_START 0x5100
#define SOUND_REG_END 0x51FF
#define INPUT_REG_START 0x5200
#define INPUT_REG_END 0x52ff
#define SYSTEM_CTRL_START 0x5300
#define SYSTEM_CTRL_END 0x53FF
#define SYSTEM_ROM_START 0x5400
#define SYSTEM_ROM_END 0x63FF

// Memory regions
typedef enum {
    REGION_ROM,
    REGION_RAM,
    REGION_VIDEO,
    REGION_SPRITE,
    REGION_PALETTE,
    REGION_SOUND,
    REGION_INPUT,
    REGION_SYSTEM_CTRL,
    REGION_SYSTEM_ROM,
    REGION_COUNT // Enum defines automatically
} MemoryRegion_t;

// Memory region structure
typedef struct {
    uint16_t start_address;
    uint16_t end_address;
    bool read_only;
    bool memory_mapped_io;
    const char* name; // Debugging purposes
} MemoryRegion;

typedef struct {
    uint16_t frequency;
    uint8_t volume;
    uint8_t waveform;
    uint8_t duty_cycle;
    uint8_t status;
    uint8_t envelope_settings;
    bool enabled;
} SoundChannel;

typedef struct {
    SoundChannel channels[4];
    uint8_t master_volume;
    uint8_t audio_control;
} SoundSystem;

/* 
 * Initializes memory with default values of 0. 
    * @param memory The memory array to initialize.
 */
void memory_init(uint8_t memory[]); 

/* 
 * Reads a byte from memory.
    * @param memory The memory array to read from.
    * @param address The address to read from.
    * @return The byte read from memory.
 */
uint8_t memory_read_byte(uint8_t memory[], uint16_t address);

/* 
 * Reads a word from memory.
    * @param memory The memory array to read from.
    * @param address The address to read from.
    * @return The word read from memory.
 */
uint16_t memory_read_word(uint8_t memory[], uint16_t address);

/* 
 * Writes given byte to memory address. Returns success of operation.
    * @param memory The memory array to write to.
    * @param address The address to write to.
    * @param data The byte to write.
    * @return true if the write was successful, false otherwise.
 */
bool memory_write_byte(uint8_t memory[], uint16_t address, uint8_t data);

/* 
 * Writes given word to memory address. Returns success of operation.
    * @param memory The memory array to write to.
    * @param address The address to write to.
    * @param data The word to write.
    * @return true if the write was successful, false otherwise.
 */
bool memory_write_word(uint8_t memory[], uint16_t address, uint16_t data);

/* 
 * Returns the region type of the given address. 
    * @param address The address to check.
    * @return The memory region type.
 */ 
MemoryRegion_t memory_get_region(uint16_t address);

/* 
 * Dumps contents stored in memory addresses.
    * @param memory The memory array to dump.
    * @param start_address The starting address to dump from.
    * @param length The number of byte sized chunks to dump.
    * @note This function is used for debugging purposes.
 */
void memory_dump(uint8_t memory[], uint16_t start_address, uint16_t length, uint16_t chunk_size);

// Function prototypes for testing purposes
void initialize_rom(uint8_t memory[]);
void load_startup(void);
void initialize_palettes(uint8_t memory[]);
bool handle_input_write(uint16_t offset, uint8_t value);
bool handle_sound_write(uint16_t offset, uint8_t value);
uint8_t handle_input_read(uint16_t offset);
uint8_t handle_sound_read(uint16_t offset);
#endif // IDN16_MEMORY_H