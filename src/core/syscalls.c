#include "idn16/cpu.h"
#include "idn16/memory.h"
#include <stdlib.h>
#include <string.h>

void syscall_clear_screen(Cpu_t* cpu) {
    // Clear tile buffer
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        memory_write_byte(cpu->memory, TILE_BUFFER_START + i, 0, true);
    }
    
    // Reset cursor position
    memory_write_word(cpu->memory, CURSOR_X_REG, 0, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, 0, true);
    
    // Return success in r1
    cpu->r[1] = 1;
}

void syscall_put_char(Cpu_t* cpu) {
    uint8_t character = cpu->r[1] & 0xFF;
    uint16_t cursor_x = memory_read_word(cpu->memory, CURSOR_X_REG);
    uint16_t cursor_y = memory_read_word(cpu->memory, CURSOR_Y_REG);
    
    if (character == '\n') {
        // Newline - move to next line
        cursor_x = 0;
        cursor_y++;
    } else {
        // Write character to tile buffer
        if (cursor_x < SCREEN_WIDTH_TILES && cursor_y < SCREEN_HEIGHT_TILES) {
            uint16_t tile_addr = TILE_BUFFER_START + (cursor_y * SCREEN_WIDTH_TILES) + cursor_x;
            memory_write_byte(cpu->memory, tile_addr, character, true);
            cursor_x++;
        }
    }
    
    // Handle screen wrapping
    if (cursor_x >= SCREEN_WIDTH_TILES) {
        cursor_x = 0;
        cursor_y++;
    }
    if (cursor_y >= SCREEN_HEIGHT_TILES) {
        cursor_y = 0; // Simple wrap to top
    }
    
    // Update cursor position
    memory_write_word(cpu->memory, CURSOR_X_REG, cursor_x, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, cursor_y, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_put_string(Cpu_t* cpu) {
    uint16_t string_addr = cpu->r[1];
    uint16_t max_length = cpu->r[2];
    
    for (uint16_t i = 0; i < max_length; i++) {
        uint8_t character = memory_read_byte(cpu->memory, string_addr + i);
        if (character == 0) break; // Null terminator
        
        // Put each character using put_char logic
        cpu->r[1] = character;
        syscall_put_char(cpu);
    }
    
    cpu->r[1] = 1; // Success
}

void syscall_get_input(Cpu_t* cpu) {
    // Read controller input
    uint8_t input = memory_read_byte(cpu->memory, INPUT_CONTROLLER1);
    cpu->r[1] = input;
}

void syscall_play_tone(Cpu_t* cpu) {
    uint16_t frequency = cpu->r[1];
    uint16_t duration = cpu->r[2];
    
    // Write to audio registers (simplified)
    memory_write_word(cpu->memory, AUDIO_REG_START, frequency, true);
    memory_write_word(cpu->memory, AUDIO_REG_START + 2, duration, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_multiply(Cpu_t* cpu) {
    int16_t a = (int16_t)cpu->r[1];
    int16_t b = (int16_t)cpu->r[2];
    int32_t result = (int32_t)a * (int32_t)b;
    
    // Return lower 16 bits in r1, upper 16 bits in r2
    cpu->r[1] = (uint16_t)(result & 0xFFFF);
    cpu->r[2] = (uint16_t)((result >> 16) & 0xFFFF);
}

void syscall_divide(Cpu_t* cpu) {
    int16_t dividend = (int16_t)cpu->r[1];
    int16_t divisor = (int16_t)cpu->r[2];
    
    if (divisor == 0) {
        cpu->r[1] = 0; // Division by zero
        cpu->r[2] = 0;
        return;
    }
    
    int16_t quotient = dividend / divisor;
    int16_t remainder = dividend % divisor;
    
    cpu->r[1] = (uint16_t)quotient;
    cpu->r[2] = (uint16_t)remainder;
}

void syscall_random(Cpu_t* cpu) {
    // Simple linear congruential generator (not cryptographically secure)
    static uint32_t seed = 1;
    seed = (seed * 1103515245 + 12345) & 0x7FFFFFFF;
    cpu->r[1] = (uint16_t)(seed & 0xFFFF);
}

void syscall_memcpy(Cpu_t* cpu) {
    uint16_t dest_addr = cpu->r[1];
    uint16_t src_addr = cpu->r[2];
    uint16_t length = cpu->r[3];
    
    for (uint16_t i = 0; i < length; i++) {
        uint8_t byte = memory_read_byte(cpu->memory, src_addr + i);
        memory_write_byte(cpu->memory, dest_addr + i, byte, false);
    }
    
    cpu->r[1] = length; // Return bytes copied
}