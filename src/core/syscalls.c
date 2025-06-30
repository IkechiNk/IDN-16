#include "idn16/cpu.h"
#include "idn16/memory.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void syscall_clear_screen(Cpu_t* cpu) {
    // Clear tile buffer with space characters
    for (int i = 0; i < SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES; i++) {
        memory_write_byte(cpu->memory, CHAR_BUFFER_START + i, 32, true); // 32 = space character
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
            uint16_t tile_addr = CHAR_BUFFER_START + (cursor_y * SCREEN_WIDTH_TILES) + cursor_x;
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
    uint16_t dividend = (uint16_t)cpu->r[1];
    uint16_t divisor = (uint16_t)cpu->r[2];

    // Division by zero
    if (divisor == 0) {
        cpu->r[1] = 0; 
        cpu->r[2] = 0;
        return;
    }
    
    uint16_t quotient = dividend / divisor;
    uint16_t remainder = dividend % divisor;
    
    cpu->r[1] = (uint16_t)quotient;
    cpu->r[2] = (uint16_t)remainder;
}

void syscall_random(Cpu_t* cpu) {
    uint16_t value = ((uint16_t)rand() & 0xFFFF);
    cpu->r[1] = value;
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

// Additional 8x8 font system calls
void syscall_set_cursor(Cpu_t* cpu) {
    uint16_t x = cpu->r[1];
    uint16_t y = cpu->r[2];
    
    // Clamp to screen bounds
    if (x >= SCREEN_WIDTH_TILES) x = SCREEN_WIDTH_TILES - 1;
    if (y >= SCREEN_HEIGHT_TILES) y = SCREEN_HEIGHT_TILES - 1;
    
    memory_write_word(cpu->memory, CURSOR_X_REG, x, true);
    memory_write_word(cpu->memory, CURSOR_Y_REG, y, true);
}

void syscall_get_cursor(Cpu_t* cpu) {
    cpu->r[1] = memory_read_word(cpu->memory, CURSOR_X_REG);
    cpu->r[2] = memory_read_word(cpu->memory, CURSOR_Y_REG);
}

void syscall_put_char_at(Cpu_t* cpu) {
    uint16_t x = cpu->r[1];
    uint16_t y = cpu->r[2];
    uint8_t character = cpu->r[3] & 0xFF;
    
    if (x < SCREEN_WIDTH_TILES && y < SCREEN_HEIGHT_TILES) {
        uint16_t tile_addr = CHAR_BUFFER_START + (y * SCREEN_WIDTH_TILES) + x;
        memory_write_byte(cpu->memory, tile_addr, character, true);
    }
}

void syscall_scroll_up(Cpu_t* cpu) {
    // Move all lines up by one
    for (uint16_t y = 0; y < SCREEN_HEIGHT_TILES - 1; y++) {
        for (uint16_t x = 0; x < SCREEN_WIDTH_TILES; x++) {
            uint16_t src_addr = CHAR_BUFFER_START + ((y + 1) * SCREEN_WIDTH_TILES) + x;
            uint16_t dest_addr = CHAR_BUFFER_START + (y * SCREEN_WIDTH_TILES) + x;
            uint8_t character = memory_read_byte(cpu->memory, src_addr);
            memory_write_byte(cpu->memory, dest_addr, character, true);
        }
    }
    
    // Clear the last line
    for (uint16_t x = 0; x < SCREEN_WIDTH_TILES; x++) {
        uint16_t addr = CHAR_BUFFER_START + ((SCREEN_HEIGHT_TILES - 1) * SCREEN_WIDTH_TILES) + x;
        memory_write_byte(cpu->memory, addr, ' ', true);
    }
}

void syscall_fill_area(Cpu_t* cpu) {
    uint16_t x = cpu->r[1];
    uint16_t y = cpu->r[2]; 
    uint16_t width = cpu->r[3];
    uint16_t height = cpu->r[4];
    uint8_t character = cpu->r[5] & 0xFF;
    
    for (uint16_t row = 0; row < height && (y + row) < SCREEN_HEIGHT_TILES; row++) {
        for (uint16_t col = 0; col < width && (x + col) < SCREEN_WIDTH_TILES; col++) {
            uint16_t addr = CHAR_BUFFER_START + ((y + row) * SCREEN_WIDTH_TILES) + (x + col);
            memory_write_byte(cpu->memory, addr, character, true);
        }
    }
}

void syscall_set_text_color(Cpu_t* cpu) {
    uint16_t fg_color = cpu->r[1];
    uint16_t bg_color = cpu->r[2];
    
    // Store colors in unused video control registers for future use
    memory_write_word(cpu->memory, VIDEO_CONTROL_START + 10, fg_color, true);
    memory_write_word(cpu->memory, VIDEO_CONTROL_START + 12, bg_color, true);
}

void syscall_print_hex(Cpu_t* cpu) {
    uint16_t number = cpu->r[1];
    
    // Convert to hex string and display
    char hex_str[5]; // "FFFF\0"
    sprintf(hex_str, "%04X", number);
    
    for (int i = 0; hex_str[i] != '\0'; i++) {
        cpu->r[1] = hex_str[i];
        syscall_put_char(cpu);
    }
}

void syscall_print_dec(Cpu_t* cpu) {
    uint16_t number = cpu->r[1];
    
    // Convert to decimal string and display
    char dec_str[6]; // "65535\0"
    sprintf(dec_str, "%u", number);
    
    for (int i = 0; dec_str[i] != '\0'; i++) {
        cpu->r[1] = dec_str[i];
        syscall_put_char(cpu);
    }
}

void syscall_set_sprite(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t x = cpu->r[2] & 0xFF;
    uint8_t y = cpu->r[3] & 0xFF;
    uint8_t tile_id = cpu->r[4] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES || tile_id > MAX_TILES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    // Validate tile coordinates (0-39 for x, 0-29 for y)
    if (x >= SCREEN_WIDTH_TILES || y >= SCREEN_HEIGHT_TILES) {
        printf("Invalid tile coordinates: x=%d, y=%d\n", x, y);
        fflush(stdout);
        cpu->r[1] = 0; // Error: invalid tile coordinates
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    memory_write_byte(cpu->memory, sprite_addr + 0, x, true);
    memory_write_byte(cpu->memory, sprite_addr + 1, y, true);
    memory_write_byte(cpu->memory, sprite_addr + 2, tile_id, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_set_palette(Cpu_t* cpu) {
    uint8_t palette_index = cpu->r[1] & 0xFF;
    uint16_t color = cpu->r[2];
    
    if (palette_index >= PALETTE_SIZE) {
        cpu->r[1] = 0; // Error: invalid palette index
        return;
    }
    
    uint16_t palette_addr = PALETTE_RAM_START + (palette_index * 2);
    memory_write_word(cpu->memory, palette_addr, color, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_move_sprite(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t new_x = cpu->r[2] & 0xFF;
    uint8_t new_y = cpu->r[3] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    // Validate tile coordinates (0-39 for x, 0-29 for y)
    if (new_x >= SCREEN_WIDTH_TILES || new_y >= SCREEN_HEIGHT_TILES) {
        cpu->r[1] = 0; // Error: invalid tile coordinates
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    memory_write_byte(cpu->memory, sprite_addr + 0, new_x, true);
    memory_write_byte(cpu->memory, sprite_addr + 1, new_y, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_set_sprite_pixel(Cpu_t* cpu) {
    uint8_t tile_id = cpu->r[1] & 0xFF;
    uint8_t pixel_x = cpu->r[2] & 0xFF;
    uint8_t pixel_y = cpu->r[3] & 0xFF;
    uint8_t palette_index = cpu->r[4] & 0xFF;
    
    // Validate tile ID: 0 = disabled sprite, 1+ = valid tiles
    if (tile_id == 0 || tile_id > MAX_TILES) {
        cpu->r[1] = 0; // Error: invalid tile ID (0 = disabled, >MAX_TILES = out of range)
        return;
    }
    
    if (pixel_x >= 8 || pixel_y >= 8) {
        cpu->r[1] = 0; // Error: pixel coordinates out of bounds (8x8 sprite)
        return;
    }
    
    // Palette index validation according to display logic:
    // 0 = transparent, 1-15 = valid colors, 16+ = use previous valid (handled in display)
    // Allow any palette_index value as display.c handles the fallback logic
    
    // Calculate pixel address within sprite data
    // tile_id=1 refers to first tile at TILESET_DATA_START, so offset by (tile_id-1)
    uint16_t sprite_data_addr = TILESET_DATA_START + ((tile_id - 1) * 64);
    uint16_t pixel_addr = sprite_data_addr + (pixel_y * 8 + pixel_x);
    
    // Set the pixel color (display.c will handle palette index 16+ fallback)
    memory_write_byte(cpu->memory, pixel_addr, palette_index, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_get_frame_count(Cpu_t* cpu) {
    // Return current frame count in r1
    cpu->r[1] = cpu->frame_count;
}

void syscall_hide_sprite(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    // Hide sprite by setting tile_id = 0 (disabled)
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    memory_write_byte(cpu->memory, sprite_addr + 2, 0, true); // tile_id = 0 (disabled)
    
    cpu->r[1] = 1; // Success
}

void syscall_get_sprite_pos(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        cpu->r[2] = 0;
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    uint8_t x = memory_read_byte(cpu->memory, sprite_addr + 0);
    uint8_t y = memory_read_byte(cpu->memory, sprite_addr + 1);
    uint8_t tile_id = memory_read_byte(cpu->memory, sprite_addr + 2);
    
    // Return tile coordinates (will be within 0-39, 0-29 if valid)
    // Off-screen or disabled sprites may have coordinates >= screen bounds
    cpu->r[1] = x; // X position (tile coordinate)
    cpu->r[2] = y; // Y position (tile coordinate)
    cpu->r[3] = tile_id; // Also return tile_id to check if sprite is disabled (0)
}

void syscall_clear_sprite_range(Cpu_t* cpu) {
    uint8_t start_id = cpu->r[1] & 0xFF;
    uint8_t end_id = cpu->r[2] & 0xFF;
    
    if (start_id >= MAX_SPRITES || end_id >= MAX_SPRITES || start_id > end_id) {
        cpu->r[1] = 0; // Error: invalid sprite range
        return;
    }
    
    // Clear sprites in range by disabling them (tile_id = 0)
    for (uint8_t id = start_id; id <= end_id; id++) {
        uint16_t sprite_addr = SPRITE_TABLE_START + (id * 3);
        memory_write_byte(cpu->memory, sprite_addr + 2, 0, true);   // tile_id = 0 (disabled)
    }
    
    cpu->r[1] = (end_id - start_id + 1); // Return number of sprites cleared
}

void syscall_check_collision(Cpu_t* cpu) {
    uint8_t sprite1_id = cpu->r[1] & 0xFF;
    uint8_t sprite2_id = cpu->r[2] & 0xFF;
    
    if (sprite1_id >= MAX_SPRITES || sprite2_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    // Get positions and tile IDs of both sprites
    uint16_t sprite1_addr = SPRITE_TABLE_START + (sprite1_id * 3);
    uint16_t sprite2_addr = SPRITE_TABLE_START + (sprite2_id * 3);
    
    uint8_t x1 = memory_read_byte(cpu->memory, sprite1_addr + 0);
    uint8_t y1 = memory_read_byte(cpu->memory, sprite1_addr + 1);
    uint8_t tile1 = memory_read_byte(cpu->memory, sprite1_addr + 2);
    uint8_t x2 = memory_read_byte(cpu->memory, sprite2_addr + 0);
    uint8_t y2 = memory_read_byte(cpu->memory, sprite2_addr + 1);
    uint8_t tile2 = memory_read_byte(cpu->memory, sprite2_addr + 2);
    
    // No collision if either sprite is disabled (tile_id = 0) or off-screen
    if (tile1 == 0 || tile2 == 0 || 
        x1 >= SCREEN_WIDTH_TILES || y1 >= SCREEN_HEIGHT_TILES ||
        x2 >= SCREEN_WIDTH_TILES || y2 >= SCREEN_HEIGHT_TILES) {
        cpu->r[1] = 0; // No collision
        return;
    }
    cpu->r[1] = ((x1 == x2) && (y1 == y2)); // Return collision result
}

void syscall_shift_sprites(Cpu_t* cpu) {
    uint8_t start_id = cpu->r[1] & 0xFF;
    uint8_t count = cpu->r[2] & 0xFF;
    int8_t dx = (int8_t)(cpu->r[3] & 0xFF);
    int8_t dy = (int8_t)(cpu->r[4] & 0xFF);
    
    if (start_id >= MAX_SPRITES || (start_id + count) > MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite range
        return;
    }
    
    // Move sprites in sequence
    for (uint8_t i = 0; i < count; i++) {
        uint8_t sprite_id = start_id + i;
        uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
        
        uint8_t current_x = memory_read_byte(cpu->memory, sprite_addr + 0);
        uint8_t current_y = memory_read_byte(cpu->memory, sprite_addr + 1);
        
        // Apply offset with tile coordinate bounds checking
        int16_t new_x = current_x + dx;
        int16_t new_y = current_y + dy;
        
        // Clamp to tile coordinate bounds (0-39, 0-29)
        if (new_x < 0) new_x = 0;
        if (new_x >= SCREEN_WIDTH_TILES) new_x = SCREEN_WIDTH_TILES - 1;
        if (new_y < 0) new_y = 0;
        if (new_y >= SCREEN_HEIGHT_TILES) new_y = SCREEN_HEIGHT_TILES - 1;
        
        memory_write_byte(cpu->memory, sprite_addr + 0, (uint8_t)new_x, true);
        memory_write_byte(cpu->memory, sprite_addr + 1, (uint8_t)new_y, true);
    }
    
    cpu->r[1] = count; // Return number of sprites moved
}

void syscall_copy_sprite(Cpu_t* cpu) {
    uint8_t src_id = cpu->r[1] & 0xFF;
    uint8_t dest_id = cpu->r[2] & 0xFF;
    
    if (src_id >= MAX_SPRITES || dest_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    uint16_t src_addr = SPRITE_TABLE_START + (src_id * 3);
    uint16_t dest_addr = SPRITE_TABLE_START + (dest_id * 3);
    
    // Copy all 3 bytes of sprite data (x, y, tile_id)
    // Note: coordinates will be in tile format (0-39, 0-29) as per updated syscalls
    for (int i = 0; i < 3; i++) {
        uint8_t byte = memory_read_byte(cpu->memory, src_addr + i);
        memory_write_byte(cpu->memory, dest_addr + i, byte, true);
    }
    
    cpu->r[1] = 1; // Success
}

void syscall_set_return_addr(Cpu_t* cpu) {
    // Set return address register (r7) to next instruction address
    cpu->r[7] = cpu->pc + 2;
    cpu->r[1] = 1; // Success
}

void syscall_move_sprite_right(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t tiles = cpu->r[2] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    uint8_t current_x = memory_read_byte(cpu->memory, sprite_addr + 0);
    
    // Calculate new position with bounds checking
    int16_t new_x = current_x + tiles;
    if (new_x >= SCREEN_WIDTH_TILES) {
        new_x = SCREEN_WIDTH_TILES - 1; // Clamp to right edge
    }
    
    memory_write_byte(cpu->memory, sprite_addr + 0, (uint8_t)new_x, true);
    cpu->r[1] = 1; // Success
}

void syscall_move_sprite_left(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t tiles = cpu->r[2] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    uint8_t current_x = memory_read_byte(cpu->memory, sprite_addr + 0);
    
    // Calculate new position with bounds checking
    int16_t new_x = current_x - tiles;
    if (new_x < 0) {
        new_x = 0; // Clamp to left edge
    }
    
    memory_write_byte(cpu->memory, sprite_addr + 0, (uint8_t)new_x, true);
    cpu->r[1] = 1; // Success
}

void syscall_move_sprite_up(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t tiles = cpu->r[2] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    uint8_t current_y = memory_read_byte(cpu->memory, sprite_addr + 1);
    
    // Calculate new position with bounds checking
    int16_t new_y = current_y - tiles;
    if (new_y < 0) {
        new_y = 0; // Clamp to top edge
    }
    
    memory_write_byte(cpu->memory, sprite_addr + 1, (uint8_t)new_y, true);
    cpu->r[1] = 1; // Success
}

void syscall_move_sprite_down(Cpu_t* cpu) {
    uint8_t sprite_id = cpu->r[1] & 0xFF;
    uint8_t tiles = cpu->r[2] & 0xFF;
    
    if (sprite_id >= MAX_SPRITES) {
        cpu->r[1] = 0; // Error: invalid sprite ID
        return;
    }
    
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_id * 3);
    uint8_t current_y = memory_read_byte(cpu->memory, sprite_addr + 1);
    
    // Calculate new position with bounds checking
    int16_t new_y = current_y + tiles;
    if (new_y >= SCREEN_HEIGHT_TILES) {
        new_y = SCREEN_HEIGHT_TILES - 1; // Clamp to bottom edge
    }
    
    memory_write_byte(cpu->memory, sprite_addr + 1, (uint8_t)new_y, true);
    cpu->r[1] = 1; // Success
}

void syscall_timer_start(Cpu_t* cpu) {
    uint16_t duration = cpu->r[1]; // Duration in milliseconds
    
    // Store current time in cpu->last_time (using frame counter as time base)
    uint32_t current_time_ms = (cpu->frame_count * 1000) / 60; // Convert frames to milliseconds
    cpu->last_time = current_time_ms;
    
    // Store duration in timer registers for reference
    memory_write_word(cpu->memory, TIMER_COUNTER_LOW, duration, true);
    
    cpu->r[1] = 1; // Success
}

void syscall_timer_query(Cpu_t* cpu) {
    // Read stop flag from r2 register
    uint16_t stop_flag = cpu->r[1];
    
    // Read timer duration from registers
    uint16_t duration = memory_read_word(cpu->memory, TIMER_COUNTER_LOW);
    
    // Calculate current time and elapsed time
    uint32_t current_time_ms = (cpu->frame_count * 1000) / 60;
    uint32_t elapsed_ms = current_time_ms - cpu->last_time;
    
    uint16_t timer_state;
    if (elapsed_ms >= duration || duration == 0) {
        // Duration met or surpassed - return 0
        cpu->r[1] = 0;
        timer_state = 1; // Timer completed
    } else {
        // Return remaining time until duration is met
        cpu->r[1] = duration - elapsed_ms;
        timer_state = 0; // Timer still running
    }
    
    // If stop flag is set, always stop/reset the timer
    if (stop_flag) {
        memory_write_word(cpu->memory, TIMER_COUNTER_LOW, 0, true);
        cpu->last_time = 0;
        timer_state = 1; // Timer stopped
    }
    
    // Return timer state in r2
    cpu->r[2] = timer_state;
}

void syscall_sleep(Cpu_t* cpu) {
    uint16_t duration = cpu->r[1]; // Sleep duration in milliseconds
    
    // Set the CPU sleep timer to the user-provided value
    cpu->sleep_timer = duration;
    
    cpu->r[1] = 1; // Success
}

