#include "display.h"
#include "core/memory.h"
#include <string.h>

// // Simple 8x8 font for text mode (just a few characters for demo)
// static const uint8_t default_font[96][8] = {
//     // Space (32)
//     {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
//     // ! (33) 
//     {0x18, 0x3C, 0x3C, 0x18, 0x18, 0x00, 0x18, 0x00},
//     // ... more characters would go here
//     // For brevity, just showing basic structure
// };

display_t* display_init(int width, int height, int scale) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }

    display_t* display = malloc(sizeof(display_t));
    if (!display) {
        printf("Failed to allocate display structure\n");
        SDL_Quit();
        return NULL;
    }

    display->width = width;
    display->height = height;

    // Create window
    display->window = SDL_CreateWindow(
        "IDN-16 Virtual Console",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width * scale,
        height * scale,
        SDL_WINDOW_SHOWN
    );

    if (!display->window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        free(display);
        SDL_Quit();
        return NULL;
    }

    // Create renderer
    display->renderer = SDL_CreateRenderer(display->window, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    
    if (!display->renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(display->window);
        free(display);
        SDL_Quit();
        return NULL;
    }

    // Create texture
    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!display->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        free(display);
        SDL_Quit();
        return NULL;
    }

    // Allocate pixel buffer
    display->pixels = malloc(width * height * sizeof(uint16_t));
    if (!display->pixels) {
        printf("Failed to allocate pixel buffer\n");
        SDL_DestroyTexture(display->texture);
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        free(display);
        SDL_Quit();
        return NULL;
    }

    // Initialize tile cache
    for (int i = 0; i < 256; i++) {
        display->tile_cache[i] = malloc(64 * sizeof(uint32_t)); // 8x8 tiles
        display->tile_cache_dirty[i] = true;
    }

    SDL_RenderSetScale(display->renderer, scale, scale);
    clear_screen_buffer(display, 0x0000);

    display->frames_rendered = 0;
    display->tiles_rendered = 0;
    display->sprites_rendered = 0;

    return display;
}

void display_destroy(display_t* display) {
    if (display) {
        // Clean up tile cache
        for (int i = 0; i < 256; i++) {
            if (display->tile_cache[i]) {
                free(display->tile_cache[i]);
            }
        }

        if (display->pixels) {
            free(display->pixels);
        }
        if (display->texture) {
            SDL_DestroyTexture(display->texture);
        }
        if (display->renderer) {
            SDL_DestroyRenderer(display->renderer);
        }
        if (display->window) {
            SDL_DestroyWindow(display->window);
        }
        
        free(display);
        SDL_Quit();
    }
}

void display_update(display_t* display, uint8_t memory[]) {
    // Read video mode from memory
    uint8_t video_mode = memory_read_byte(memory, VIDEO_MODE_REG);
    
    // Clear screen
    clear_screen_buffer(display, get_palette_color(memory, 0));

    // Render based on video mode
    switch (video_mode) {
        case VIDEO_MODE_TEXT:
            render_text_mode(display, memory);
            break;
        case VIDEO_MODE_TILES:
            render_tile_mode(display, memory);
            break;
        case VIDEO_MODE_MIXED:
            render_tile_mode(display, memory);
            render_mixed_mode(display, memory);
            break;
        default:
            render_text_mode(display, memory);
            break;
    }

    // Update texture and present
    SDL_UpdateTexture(display->texture, NULL, display->pixels, 
                     display->width * sizeof(uint16_t));
    SDL_RenderClear(display->renderer);
    SDL_RenderCopy(display->renderer, display->texture, NULL, NULL);
    SDL_RenderPresent(display->renderer);

    display->frames_rendered++;
}

void render_text_mode(display_t* display, uint8_t memory[]) {
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 40; x++) {
            uint16_t tile_addr = TILE_BUFFER_START + (y * 40 + x);
            uint8_t ch = memory_read_byte(memory, tile_addr);
            
            if (ch >= 32 && ch < 127) {
                // Render character using built-in font
                for (int py = 0; py < 8; py++) {
                    for (int px = 0; px < 8; px++) {
                        int screen_x = x * 8 + px;
                        int screen_y = y * 8 + py;
                        uint16_t color = 0xFFFF; // White for now
                        display->pixels[screen_y * display->width + screen_x] = color;
                    }
                }
            }
        }
    }
}

void render_tile_mode(display_t* display, uint8_t memory[]) {
    int16_t scroll_x = (int16_t)memory_read_word(memory, SCROLL_X_REG);
    int16_t scroll_y = (int16_t)memory_read_word(memory, SCROLL_Y_REG);

    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 40; x++) {
            uint16_t tile_addr = TILE_BUFFER_START + (y * 40 + x);
            uint8_t tile_id = memory_read_byte(memory, tile_addr);

            int screen_x = x * 8 - (scroll_x % 8);
            int screen_y = y * 8 - (scroll_y % 8);

            render_tile_to_buffer(display, memory, tile_id, screen_x, screen_y);
        }
    }
}

void render_mixed_mode(display_t* display, uint8_t memory[]) {
    // Render sprites on top of tiles
    for (int i = 0; i < 64; i++) {
        uint16_t sprite_addr = SPRITE_TABLE_START + (i * 4);
        uint8_t sprite_x = memory_read_byte(memory, sprite_addr + 0);
        uint8_t sprite_y = memory_read_byte(memory, sprite_addr + 1);
        // uint8_t tile_id = memory_read_byte(memory, sprite_addr + 2);

        if (sprite_x < display->width && sprite_y < display->height) {
            render_sprite_to_buffer(display, memory, i);
        }
    }
}

void render_tile_to_buffer(display_t* display, uint8_t memory[], int tile_id, int screen_x, int screen_y) {
    if (display->tile_cache_dirty[tile_id]) {
        update_tile_cache(display, memory, tile_id);
    }

    // Simple tile rendering - just fill with a color based on tile_id for now
    uint16_t color = get_palette_color(memory, tile_id % 16);
    
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            int final_x = screen_x + px;
            int final_y = screen_y + py;

            if (final_x >= 0 && final_x < display->width &&
                final_y >= 0 && final_y < display->height) {
                display->pixels[final_y * display->width + final_x] = color;
            }
        }
    }
    
    display->tiles_rendered++;
}

void render_sprite_to_buffer(display_t* display, uint8_t memory[], int sprite_index) {
    uint16_t sprite_addr = SPRITE_TABLE_START + (sprite_index * 4);
    uint8_t sprite_x = memory_read_byte(memory, sprite_addr + 0);
    uint8_t sprite_y = memory_read_byte(memory, sprite_addr + 1);
    uint8_t tile_id = memory_read_byte(memory, sprite_addr + 2);

    // Render 16x16 sprite as 4 tiles
    for (int ty = 0; ty < 2; ty++) {
        for (int tx = 0; tx < 2; tx++) {
            int current_tile = tile_id + (ty * 16) + tx;
            int tile_x = sprite_x + tx * 8;
            int tile_y = sprite_y + ty * 8;
            
            render_tile_to_buffer(display, memory, current_tile, tile_x, tile_y);
        }
    }
    
    display->sprites_rendered++;
}

void update_tile_cache(display_t* display, uint8_t memory[], int tile_id) {
    // For now, just mark as clean
    display->tile_cache_dirty[tile_id] = false;
}

uint16_t get_palette_color(uint8_t memory[], int palette_index) {
    if (palette_index >= 64) palette_index = 0;
    uint16_t color_addr = PALETTE_RAM_START + (palette_index * 2);
    return memory_read_word(memory, color_addr);
}

void clear_screen_buffer(display_t* display, uint16_t color) {
    for (int i = 0; i < display->width * display->height; i++) {
        display->pixels[i] = color;
    }
}

uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}