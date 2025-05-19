#include "memory.h"
#include "keyboard.c"

SoundSystem sound_system;

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

void initialize_rom(uint8_t memory[]) {
    // Load ROM data into memory
    FILE *rom_file = fopen("rom.bin", "rb");
    if (rom_file) {
        fread(&memory[ROM_START], 1, ROM_SIZE, rom_file);
        fclose(rom_file);
    } else {
        fprintf(stderr, "Alert: No rom loaded.\n");
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

void initialize_palettes(uint8_t memory[]) {
    uint16_t default_palette[16] = {
        0x0000, // Black
        0xFFFF, // White
        0xF800, // Red
        0x07E0, // Green
        0x001F // Blue
    };
    memcpy(&memory[PALETTE_MEM_START], default_palette, sizeof(default_palette));
}

void memory_init(uint8_t memory[]) {
    memset(memory, 0, MEMORY_SIZE);

    initialize_rom(memory);
    initialize_palettes(memory);
    // TODO: Initialize system rom with handlers and functions
    load_startup();
}

uint8_t get_controller1_button_state() {
    uint8_t button_state = 0;
    
    // Check real keyboard/gamepad and set bits accordingly
    if (is_key_pressed(SDL_SCANCODE_F))         button_state |= 0x01; // A button
    if (is_key_pressed(SDL_SCANCODE_G))         button_state |= 0x02; // B button
    if (is_key_pressed(SDL_SCANCODE_TAB))       button_state |= 0x04; // Start
    if (is_key_pressed(SDL_SCANCODE_LSHIFT))    button_state |= 0x08; // Select
    if (is_key_pressed(SDL_SCANCODE_W))         button_state |= 0x10; // Up
    if (is_key_pressed(SDL_SCANCODE_S))         button_state |= 0x20; // Down
    if (is_key_pressed(SDL_SCANCODE_A))         button_state |= 0x40; // Left
    if (is_key_pressed(SDL_SCANCODE_D))         button_state |= 0x80; // Right
    
    return button_state;
}

uint8_t get_controller2_button_state() {
    uint8_t button_state = 0;
    
    // Check real keyboard/gamepad and set bits accordingly
    if (is_key_pressed(SDL_SCANCODE_J))         button_state |= 0x01; // A button
    if (is_key_pressed(SDL_SCANCODE_K))         button_state |= 0x02; // B button
    if (is_key_pressed(SDL_SCANCODE_RETURN))    button_state |= 0x04; // Start
    if (is_key_pressed(SDL_SCANCODE_RSHIFT))    button_state |= 0x08; // Select
    if (is_key_pressed(SDL_SCANCODE_UP))        button_state |= 0x10; // Up
    if (is_key_pressed(SDL_SCANCODE_DOWN))      button_state |= 0x20; // Down
    if (is_key_pressed(SDL_SCANCODE_LEFT))      button_state |= 0x40; // Left
    if (is_key_pressed(SDL_SCANCODE_RIGHT))     button_state |= 0x80; // Right
    
    return button_state;
}


uint8_t handle_input_read(uint16_t offset) {
    switch (offset) {
        case 0x00: // Controller 1 state
            return get_controller1_button_state();
            break;
        case 0x01: // Controller 2 state
            return get_controller2_button_state();
            break;
        default:
            return 0x00; // Invalid offset
            break;
    }
}

uint8_t handle_sound_read(uint16_t offset) {
    uint8_t channel = offset / 16;
    uint8_t reg = offset % 16;

    if (channel >= 4) {
        fprintf(stderr, "Error: Invalid sound channel %d\n", channel);
        return 0;
    }
    
    // Map the memory address to the sound system
    switch (reg) {
        case 0x0: 
            return sound_system.channels[channel].frequency & 0xFF;
            break;
        case 0x1: 
            return (sound_system.channels[channel].frequency >> 8) & 0xFF;
            break;
        case 0x2: 
            return sound_system.channels[channel].volume;
            break;
        default:
            printf("Read from sound channel: 0x%04X:0x%04X\n", channel, reg);
            break;
    }
    return 0;
}


bool handle_input_write(uint16_t offset, uint8_t value) {
    switch (offset) {
        case 0x00: 
        case 0x01:
            printf("Write to read-only input register: 0x%04X\n", offset);
            return false; 
            break;
        default:    
            printf("(nothing actually written) Write to input register: 0x%04X = %u\n", offset, value);
            return true; 
            break;       
    }
}

bool handle_sound_write(uint16_t offset, uint8_t value) {
    uint8_t channel = offset / 16;
    uint8_t reg = offset % 16;
    
    // Update sound structure based on memory write
    switch (reg) {
        case 0x0:
            sound_system.channels[channel].frequency = 
                (sound_system.channels[channel].frequency & 0xFF00) | value;
            // Call function to update sound frequency
            return true;
            break;
        case 0x1:
            sound_system.channels[channel].frequency = 
                (sound_system.channels[channel].frequency & 0x00FF) | (value << 8);
            // Call function to update sound frequency
            return true;
            break;
        case 0x2:
            sound_system.channels[channel].volume = value;
            // Call function to update sound volume
            return true;
            break;
        default:
            printf("(nothing actually written) Write to sound channel: 0x%04X:0x%04X = %u\n", channel, reg, value);
            return true;
            break;
    }
}

uint8_t memory_read_byte(uint8_t memory[], uint16_t address) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->memory_mapped_io && region_type != REGION_COUNT) {
        switch (region_type)
        {
        case REGION_INPUT:
            return handle_input_read(address - INPUT_REG_START);
            break;
        case REGION_SOUND:
            return handle_sound_read(address - SOUND_REG_START);
            break;
        default:
            printf("Read from memory-mapped I/O: 0x%04X\n", address);
            return 0x00;
            break;
        }
    }
    else {
        return memory[address];
    }
}

uint16_t memory_read_word(uint8_t memory[], uint16_t address) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->memory_mapped_io && region_type != REGION_COUNT) {
        switch (region_type)
        {
        case REGION_INPUT:
            return handle_input_read(address - INPUT_REG_START);
            break;
        case REGION_SOUND:
            return handle_sound_read(address - SOUND_REG_START);
            break;
        default:
            printf("Read from memory-mapped I/O: 0x%04X\n", address);
            return 0x00;
            break;
        }
    }
    else {
        if (address == MEMORY_SIZE - 1) {
            fprintf(stderr, "Error: Attempt to read word from odd address.\n");
            return 0;
        }
        uint16_t data = (uint16_t)memory[address] << 8;
        data |= memory[address + 1];
        return data;
    }
}

bool memory_write_byte(uint8_t memory[], uint16_t address, uint8_t data) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->read_only) {
        fprintf(stderr, "Error: Attempt to write to read-only memory.\n");
        return false;
    }
    if (region->memory_mapped_io && region_type != REGION_COUNT) {
        bool success = false;
        switch (region_type)
        {
        case REGION_INPUT:
            success = handle_input_write(address - INPUT_REG_START, data);
            break;
        case REGION_SOUND:
            success = handle_sound_write(address - SOUND_REG_START, data);
            break;
        default:
            printf("Write to memory-mapped I/O: 0x%04X\n", address);
            break;
        }
        return success;
    }
    else {
        memory[address] = data;
        return true;
    }
}

bool memory_write_word(uint8_t memory[], uint16_t address, uint16_t data) {
    MemoryRegion_t region_type = memory_get_region(address);
    const MemoryRegion* region = &memory_regions[region_type];
    if (region->read_only) {
        fprintf(stderr, "Error: Attempt to write to read-only memory.\n");
        return false;
    }
    if (region->memory_mapped_io && region_type != REGION_COUNT) {
        bool success = false;
        switch (region_type)
        {
        case REGION_INPUT:
            success = handle_input_write(address - INPUT_REG_START, data);
            break;
        case REGION_SOUND:
            success = handle_sound_write(address - SOUND_REG_START, data);
            break;
        default:
            printf("Write to memory-mapped I/O: 0x%04X\n", address);
            break;
        }
        return success;
    }
    else {
        if (address == MEMORY_SIZE - 1) {
            fprintf(stderr, "Error: Attempt to write word to odd address.\n");
            return false;
        }
        memory[address] = (uint8_t)(data >> 8);
        memory[address + 1] = (uint8_t)(data & 0x00FF);
        return true;
    }
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
