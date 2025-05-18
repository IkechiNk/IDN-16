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
    REGION_SYSTEM,
    REGION_COUNT // Enum defines automatically
} MemoryRegionType;

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

/* Initializes memory with default values of 0, */
void memory_init(void); 
/* Reads a byte from memory.*/
uint8_t memory_read_byte(uint16_t address);
/* Reads a word from memory.*/
uint16_t memory_read_word(uint16_t address);
/* Writes given byte to memory address. Returns success of operation. */
bool memory_write_byte(uint16_t address, uint8_t data);
/* Writes given word to memory address. Returns success of operation. */
bool memory_write_word(uint16_t address, uint16_t data);
/* Returns the region type of the given address. */ 
MemoryRegionType memory_get_region(uint16_t address);
/* Dumps words stored in memory from address start to stdout.*/
void memory_dump(uint16_t start, uint16_t length);