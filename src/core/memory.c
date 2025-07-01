#include "idn16/memory.h"
#include "stdio.h"

// Memory regions configuration
static const MemoryRegion memory_regions[REGION_COUNT] = {
    {USER_ROM_START, USER_ROM_END, true, false, "User ROM"},
    {RAM_START, RAM_END, false, false, "RAM"},
    {VIDEO_RAM_START, VIDEO_RAM_END, false, true, "Video Memory"},
    {AUDIO_REG_START, AUDIO_REG_END, false, true, "Audio Registers"},
    {INPUT_REG_START, INPUT_REG_END, false, true, "Input Registers"},
    {SYSTEM_CTRL_START, SYSTEM_CTRL_END, false, true, "System Control"},
    {SYSCALL_BASE, SYSCALL_END, true, false, "System Calls"},
};

static bool is_little_endian(void) {
    uint16_t value = 0x0001;
    return *((uint8_t*)&value);
}

bool load_user_rom(uint8_t memory[], FILE* rom) {
    // Get file size first
    fseek(rom, 0, SEEK_END);
    size_t file_size = ftell(rom);
    rewind(rom);
    
    // Read up to ROM size limit (32KB)
    size_t max_read = (USER_ROM_END - USER_ROM_START + 1);
    size_t bytes_to_read = (file_size < max_read) ?  file_size : max_read;
    
    fread(memory + USER_ROM_START, 1, bytes_to_read, rom);
    return true;
}

void memory_init(uint8_t memory[]) {
    // Clear all memory
    memset(memory, 0, MEMORY_SIZE);
    
    // Initialize system control registers
    memory[SYSTEM_STATUS_REG] = 0x01; // System running
    memory[INTERRUPT_CONTROL_REG] = 0x00; // Interrupts disabled initially
    
    // Initialize video system
    initialize_video_memory(memory);
    initialize_default_palette(memory);
    
    // Initialize stack pointer to top of RAM
    memory_write_word(memory, SYSTEM_CTRL_START + 10, RAM_END, true);
    
    printf("IDN-16 System initialized (simplified - no system ROM)\n");
}

void initialize_video_memory(uint8_t memory[]) {
    // Initialize video system (no video mode - dual architecture)
    
    // Clear cursor positions
    memory_write_word(memory, CURSOR_X_REG, 0, true);
    memory_write_word(memory, CURSOR_Y_REG, 0, true);
    
    // Clear tile buffer
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        memory_write_byte(memory, CHAR_BUFFER_START + i, 0, true);
    }
    
    // Clear sprite table (3 bytes per sprite: X, Y, Tile_ID)
    for (int i = 0; i < MAX_SPRITES * 3; i++) {
        memory_write_byte(memory, SPRITE_TABLE_START + i, 0, true);
    }

    // Clear tile set data (64 bytes per tile, 105 tiles)
    for (int i = 0; i < MAX_TILES * TILE_SIZE * TILE_SIZE; i++) {
        memory_write_byte(memory, TILESET_DATA_START + i, 17, true);
    }
}

void initialize_default_palette(uint8_t memory[]) {
    // 16-color palette (RGB565 format)
    const uint16_t default_colors[16] = {
        0x0000, // 0 - Black
        0x000F, // 1 - Dark Blue
        0x03E0, // 2 - Dark Green  
        0x03EF, // 3 - Dark Cyan
        0x7C00, // 4 - Dark Red
        0x7C0F, // 5 - Dark Magenta
        0x7FE0, // 6 - Brown/Dark Yellow
        0x7BEF, // 7 - Light Gray
        0x39C7, // 8 - Dark Gray
        0x001F, // 9 - Blue
        0x07E0, // 10 - Green
        0x07FF, // 11 - Cyan
        0xF800, // 12 - Red
        0xF81F, // 13 - Magenta
        0xFFE0, // 14 - Yellow
        0xFFFF, // 15 - White
    };
    
    // Write all 16 colors to palette RAM
    for (int i = 0; i < PALETTE_SIZE; i++) {
        memory_write_word(memory, PALETTE_RAM_START + (i * 2), default_colors[i], true);
    }
}


uint8_t memory_read_byte(uint8_t memory[], uint16_t address) {
    return memory[address];
}

uint16_t memory_read_word(uint8_t memory[], uint16_t address) {
    if (address == MEMORY_SIZE - 1) {
        fprintf(stderr, "Error: Attempt to read word from odd address.\n");
        return 0;
    }
    uint16_t data;
    if (is_little_endian()) {
        data = (memory[address + 1] << 8) | memory[address];
    } else {
        data = (memory[address] << 8) | memory[address + 1];
    }
    return data;
}

bool memory_write_byte(uint8_t memory[], uint16_t address, uint8_t data, bool privileged) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->read_only && !privileged) {
        fprintf(stderr, "Error: Attempt to write byte to read-only memory. REGION: %s\n", region->name);
        exit(1);
    }
    memory[address] = data;
    return true;
}

bool memory_write_word(uint8_t memory[], uint16_t address, uint16_t data, bool privileged) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->read_only && !privileged) {
        fprintf(stderr, "Error: Attempt to write word to read-only memory. REGION: %s\n", region->name);
        exit(1);
    }
    if (address == MEMORY_SIZE - 1) {
        fprintf(stderr, "Error: Attempt to write word to odd address.\n");
        exit(1);
    }
    if (is_little_endian()) {
        memory[address] = (uint8_t)(data & 0x00FF);
        memory[address + 1] = (uint8_t)(data >> 8);
    } else {
        memory[address ] = (uint8_t)(data >> 8);
        memory[address + 1] = (uint8_t)(data & 0x00FF);
    }
    return true;
}

MemoryRegion_t memory_get_region(uint16_t address) {
    for (int i = 0; i < REGION_COUNT; i++) {
        if (address >= memory_regions[i].start_address && address <= memory_regions[i].end_address) {
            return (MemoryRegion_t) i;
        }
    }
    return REGION_COUNT; // Invalid region
}

void memory_dump(uint8_t memory[], uint16_t start_address, uint16_t length, uint16_t chunk_size) {
    for (int i = 0; i < length; i++) {
        uint16_t addr = start_address + (i * chunk_size);
        printf("\n0x%04X:", addr);
        for (int j = 0; j < chunk_size; j++) {
            printf(" %02X", memory[addr + (chunk_size - j - 1)]);
        }
    }
    printf("\n");
}