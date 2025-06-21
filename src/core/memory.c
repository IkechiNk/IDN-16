#include "idn16/memory.h"
#include "stdio.h"

// Memory regions configuration
static const MemoryRegion memory_regions[REGION_COUNT] = {
    {USER_ROM_START, USER_ROM_END, true, false, "User ROM"},
    {STACK_START, STACK_END, false, false, "Stack"},
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

bool load_user_rom(uint8_t memory[], const char* rom) {
    char full_rom_loc[14 + strlen(rom) + 1];
    sprintf(full_rom_loc, "resources/roms/%s", rom);
    FILE* fRom = fopen(full_rom_loc, "rb");
    if (!fRom) {
        return false;
    }
    // Get file size first
    fseek(fRom, 0, SEEK_END);
    size_t file_size = ftell(fRom);
    rewind(fRom);
    
    // Read up to ROM size limit (32KB)
    size_t max_read = (USER_ROM_END - USER_ROM_START + 1);
    size_t bytes_to_read = (file_size < max_read) ?  file_size : max_read;
    
    fread(memory + USER_ROM_START, 1, bytes_to_read, fRom);
    fclose(fRom);
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
    initialize_default_tileset(memory);
    
    // Initialize stack pointer to top of stack
    memory_write_word(memory, SYSTEM_CTRL_START + 10, STACK_END, true);
    
    printf("IDN-16 System initialized (simplified - no system ROM)\n");
}

void initialize_video_memory(uint8_t memory[]) {
    // Set initial video mode to text
    memory_write_byte(memory, VIDEO_MODE_REG, VIDEO_MODE_TEXT, true);
    
    // Clear scroll registers
    memory_write_word(memory, SCROLL_X_REG, 0, true);
    memory_write_word(memory, SCROLL_Y_REG, 0, true);
    
    // Clear tile buffer
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        memory_write_byte(memory, TILE_BUFFER_START + i, 0, true);
    }
    
    // Clear sprite table
    for (int i = 0; i < MAX_SPRITES * 4; i++) {
        memory_write_byte(memory, SPRITE_TABLE_START + i, 0, true);
    }
}

void initialize_default_palette(uint8_t memory[]) {
    // Simple 16-color palette
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
    
    for (int i = 0; i < 16; i++) {
        memory_write_word(memory, PALETTE_RAM_START + (i * 2), default_colors[i], true);
    }
    
    // Fill remaining palette entries with variations
    for (int i = 16; i < PALETTE_SIZE; i++) {
        uint16_t base_color = default_colors[i % 16];
        // Darken the color slightly for variations
        uint16_t variant = ((base_color >> 1) & 0x7BEF);
        memory_write_word(memory, PALETTE_RAM_START + (i * 2), variant, true);
    }
}

void initialize_default_tileset(uint8_t memory[]) {
    // Font tiles for ASCII characters 32-126 (95 characters total)
    // Each tile is 8x8 pixels, stored as 32 bytes (4 bits per pixel)
    
    // Simple font data - 8 bytes per character (1 bit per pixel, expanded to 4-bit)
    // ASCII 32 (space) = Tile 0
    create_font_tile(memory, 0, (uint8_t[]){0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00});
    
    // ASCII 33 (!) = Tile 1  
    create_font_tile(memory, 1, (uint8_t[]){0x18, 0x18, 0x18, 0x18, 0x00, 0x18, 0x18, 0x00});
    
    // ASCII 45 (-) = Tile 13
    create_font_tile(memory, 13, (uint8_t[]){0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00});
    
    // ASCII 49 (1) = Tile 17
    create_font_tile(memory, 17, (uint8_t[]){0x18, 0x38, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00});
    
    // ASCII 54 (6) = Tile 22  
    create_font_tile(memory, 22, (uint8_t[]){0x3C, 0x60, 0x60, 0x7C, 0x66, 0x66, 0x3C, 0x00});
    
    // ASCII 68 (D) = Tile 36
    create_font_tile(memory, 36, (uint8_t[]){0x7C, 0x66, 0x66, 0x66, 0x66, 0x66, 0x7C, 0x00});
    
    // ASCII 73 (I) = Tile 41
    create_font_tile(memory, 41, (uint8_t[]){0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x7E, 0x00});
    
    // ASCII 78 (N) = Tile 46
    create_font_tile(memory, 46, (uint8_t[]){0x66, 0x76, 0x7E, 0x6E, 0x66, 0x66, 0x66, 0x00});
    
    // Fill remaining tiles with a default pattern for unsupported characters
    for (int i = 95; i < 256; i++) {
        create_font_tile(memory, i, (uint8_t[]){0xFF, 0x81, 0x81, 0x81, 0x81, 0x81, 0xFF, 0x00});
    }
}

void create_font_tile(uint8_t memory[], int tile_id, uint8_t bitmap[8]) {
    uint16_t tile_addr = TILESET_DATA_START + (tile_id * 32);
    
    // Convert 1-bit bitmap to 4-bit tile data
    for (int y = 0; y < 8; y++) {
        uint8_t row = bitmap[y];
        for (int x = 0; x < 4; x++) { // 2 pixels per byte
            uint8_t pixel1 = (row & (1 << (7 - x * 2))) ? 0xF : 0x0;
            uint8_t pixel2 = (row & (1 << (6 - x * 2))) ? 0xF : 0x0;
            uint8_t combined = (pixel1 << 4) | pixel2;
            memory_write_byte(memory, tile_addr + (y * 4) + x, combined, true);
        }
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
        fprintf(stderr, "Error: Attempt to write to read-only memory. REGION: %s\n", region->name);
        exit(1);
    }
    memory[address] = data;
    return true;
}

bool memory_write_word(uint8_t memory[], uint16_t address, uint16_t data, bool privileged) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->read_only && !privileged) {
        fprintf(stderr, "Error: Attempt to write to read-only memory. REGION: %s\n", region->name);
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