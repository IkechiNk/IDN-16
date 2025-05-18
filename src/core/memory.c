#include "include/memory.h"

// Memory array
uint16_t memory[MEMORY_SIZE];

// Memory regions
static const MemoryRegion memory_regions[REGION_COUNT] = {
    {ROM_START, ROM_END, true, false, "ROM"},
    {RAM_START, RAM_END, false, false, "RAM"},
    {VIDEO_MEM_START, VIDEO_MEM_END, false, true, "Video Memory"},
    {SPRITE_MEM_START, SPRITE_MEM_END, false, true, "Sprite Memory"},
    {PALETTE_MEM_START, PALETTE_MEM_END, false, true, "Palette Memory"},
    {SOUND_REG_START, SOUND_REG_END, false, true, "Sound Registers"},
    {INPUT_REG_START, INPUT_REG_END, false, true, "Input Registers"},
    {SYSTEM_CTRL_START, SYSTEM_CTRL_END, false, true, "System Control Registers"},
    {SYSTEM_ROM_START, SYSTEM_ROM_END, true, false, "System ROM"},
};

void initialize_rom(void) {
    // Load ROM data into memory
    FILE *rom_file = fopen("rom.bin", "rb");
    if (rom_file) {
        fread(&memory[ROM_START], 1, ROM_SIZE, rom_file);
        fclose(rom_file);
    } else {
        fprintf(stderr, "Error: Could not open ROM file.\n");
    }
}

void load_startup(void) {
    // Placeholder for startup screen data
    return;
}

// For use later when using graphic libraries
// uint32_t color16_to_color32(uint16_t color) {
//     uint32_t r = (color >> 11) & 0b11111;
//     uint32_t g = (color >> 5) & 0x111111;
//     uint32_t b = color & 0x11111;

//     r = (r << 3) | (r >> 2);
//     g = (g << 2) | (g >> 4);
//     b  = (b << 3) | (b >> 2);
//     return (r << 16) | (g << 8) | b;
// }

void initialize_palettes(void) {
    uint16_t default_palette[16] = {
        0x0000, // Black
        0xFFFF, // White
        0xF800, // Red
        0x07E0, // Green
        0x001F // Blue
    };
    memcpy(&memory[PALETTE_MEM_START], default_palette, sizeof(default_palette));
}

void memory_init(void) {
    memset(memory, 0, MEMORY_SIZE);

    initialize_rom();
    initialize_palettes();
    initialize_system_control();
    // TODO: Initialize system rom with handlers and functions
    load_startup();
}

uint8_t memory_read_byte(uint16_t address) {
    if (address < MEMORY_SIZE) {
        return memory[address] & 0xFF;
    } else {
        fprintf(stderr, "Error: Address out of bounds.\n");
        return 0;
    }
}

uint16_t memory_read_word(uint16_t address) {
    if (address < MEMORY_SIZE - 1) {
        return memory[address];
    } else {
        fprintf(stderr, "Error: Address out of bounds.\n");
        return 0;
    }
}

bool memory_write_byte(uint16_t address, uint8_t data) {
    if (address < MEMORY_SIZE) {
        MemoryRegionType region = memory_get_region(address);
        if (memory_regions[region].read_only == false) {
            memory[address] = (memory[address] & 11110000) ^ data;
            return true;
        } else {
            fprintf(stderr, "Error: Attempt to write to read-only memory.\n");
            return false;
        }
    } else {
        fprintf(stderr, "Error: Address out of bounds.\n");
        return false;
    }
}

bool memory_write_word(uint16_t address, uint16_t data) {
    if (address < MEMORY_SIZE) {
        MemoryRegionType region = memory_get_region(address);
        if (memory_regions[region].read_only == false) {
            memory[address] = data;
            return true;
        } else {
            fprintf(stderr, "Error: Attempt to write to read-only memory.\n");
            return false;
        }
    } else {
        fprintf(stderr, "Error: Address out of bounds.\n");
        return false;
    }
}

MemoryRegionType memory_get_region(uint16_t address) {
    for (int i = 0; i < REGION_COUNT; i++) {
        if (address >= memory_regions[i].start_address && address <= memory_regions[i].end_address) {
            return (MemoryRegionType) i;
        }
    }
    return REGION_COUNT; // Invalid region
}

void memory_dump(uint16_t start, uint16_t length) {
    for (int i = start; i < start + length; i++) {
        if (i >= MEMORY_SIZE) {
            printf("\nReached end of memory.");
            break;
        }
        if (i % 16 == 0) {
            printf("\n%04X: ", i);
        }
        printf("%02X ", memory[i]);
    }
    printf("\n");
}
