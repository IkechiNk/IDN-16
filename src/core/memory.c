#include "idn16/memory.h"
#include "stdio.h"

// Memory regions configuration
static const MemoryRegion memory_regions[REGION_COUNT] = {
    {ROM_START, ROM_END, true, false, "User ROM"},
    {RAM_START, RAM_END, false, false, "RAM"},
    {VIDEO_RAM_START, VIDEO_RAM_END, false, true, "Video Memory"},
    {AUDIO_REG_START, AUDIO_REG_END, false, true, "Audio Registers"},
    {INPUT_REG_START, INPUT_REG_END, false, true, "Input Registers"},
    {SYSTEM_CTRL_START, SYSTEM_CTRL_END, false, true, "System Control"},
    {SYSTEM_ROM_START, SYSTEM_ROM_END, true, false, "System ROM"},
};

static bool is_little_endian(void) {
    uint16_t value = 0x0001;
    return *((uint8_t*)&value);
}

bool load_rom(uint8_t memory[], const char* rom) {
    char full_rom_loc[14 + strlen(rom) + 1];
    sprintf(full_rom_loc, "src/core/roms/%s", rom);
    FILE* fRom = fopen(full_rom_loc, "rb");
    if (!fRom) {
        return false;
    }
    // Get file size first
    fseek(fRom, 0, SEEK_END);
    size_t file_size = ftell(fRom);
    rewind(fRom);
    
    // Read up to ROM size limit
    size_t max_read = (ROM_END - ROM_START + 1); // 0x2000 bytes
    size_t bytes_to_read = (file_size < max_read) ?  file_size : max_read;
    
    fread(memory, 1, bytes_to_read, fRom);
    fclose(fRom);
    return true;
}

void memory_init(uint8_t memory[]) {
    // Clear all memory
    memset(memory, 0, MEMORY_SIZE);
    memset(memory, 0b11000, ROM_END - ROM_START + 1);
    
    // Initialize system control registers
    memory[SYSTEM_STATUS_REG] = 0x01; // System running
    memory[INTERRUPT_CONTROL_REG] = 0x00; // Interrupts disabled initially
    
    // Initialize video system
    initialize_video_memory(memory);
    initialize_default_palette(memory);
    initialize_default_tileset(memory);
    
    // Load system ROM with built-in functions
    load_system_rom(memory);
    
    printf("IDN-16 System initialized\n");
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
    // Create simple default tiles
    // Each tile is 8x8 pixels, but we'll store them as 32 bytes (8x8/2, 4 bits per pixel)
    
    // Tile 0: Empty (all zeros - already cleared)
    
    // Tile 1: Solid fill
    for (int i = 0; i < 32; i++) {
        memory_write_byte(memory, TILESET_DATA_START + 32 + i, 0x11, true);
    }
    
    // Tile 2: Checkerboard
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 4; x++) { // 4 bytes per row
            uint8_t pattern = ((x + y) % 2) ? 0x12 : 0x21;
            memory_write_byte(memory, TILESET_DATA_START + 64 + (y * 4) + x, pattern, true);
        }
    }
    
    // Tile 3: Border
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 4; x++) {
            uint8_t pattern = 0x00;
            if (y == 0 || y == 7 || x == 0 || x == 3) {
                pattern = 0x33;
            }
            memory_write_byte(memory, TILESET_DATA_START + 96 + (y * 4) + x, pattern, true);
        }
    }
}

void load_system_rom(uint8_t memory[]) {
    // For now, create minimal system ROM with simple function stubs
    // In a full implementation, this would load actual system ROM functions
    
    // System ROM function addresses - just put RET instructions for now
    uint16_t system_functions[] = {
        0x4300, // SYS_CLEAR_SCREEN
        0x4302, // SYS_PUT_CHAR
        0x4304, // SYS_PUT_STRING
        0x4306, // SYS_SET_CURSOR
        0x4334, // SYS_SET_VIDEO_MODE
        0x4312, // SYS_SET_TILE
        0x4314, // SYS_CREATE_SPRITE
        0x4316, // SYS_MOVE_SPRITE
        0x4330, // SYS_SET_PALETTE
        0x4336, // SYS_SET_SCROLL
        0x4400, // SYS_PLAY_TONE
        0x4408, // SYS_STOP_CHANNEL
        0x4500, // SYS_GET_KEYS
        0x4502, // SYS_WAIT_KEY
        0x4600, // SYS_MULTIPLY
        0x4602, // SYS_DIVIDE
        0x460C, // SYS_RANDOM
        0x4700, // SYS_MEMCPY
        0x4702, // SYS_MEMSET
        0x4706, // SYS_STRLEN
        0       // End array and allow for easier expansion
    };
    
    // Install RET instructions at each function address
    for (int i = 0; system_functions[i] != 0; i++) {
        uint16_t addr = system_functions[i];
        memory_write_word(memory, addr, 0xC000, true); // RET instruction
    }
    
    printf("System ROM loaded with function stubs\n");
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