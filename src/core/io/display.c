#include "idn16/io/display.h"
#include "idn16/memory.h"
#include <string.h>
#include "font8x8/font8x8_basic.h"

display_t* display_init(int width, int height, int scale, uint8_t memory[], SDL_Renderer *renderer, TTF_Font *font) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }

    if (TTF_Init() == -1) {
        printf("Failed to initialize SDL_TTF: %s\n", SDL_GetError());
        return NULL;
    }

    display_t* display = malloc(sizeof(display_t));
    if (!display) {
        printf("Failed to allocate display structure\n");
        return NULL;
    }

    display->width = width;
    display->height = height;
    display->scale = scale;
    display->renderer = renderer;
    display->memory = memory;

    // Create texture for text rendering
    display->texture = SDL_CreateTexture(
        display->renderer,
        SDL_PIXELFORMAT_RGB565,
        SDL_TEXTUREACCESS_STREAMING,
        width,
        height
    );

    if (!display->texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        free(display);
        return NULL;
    }

    // Allocate pixel buffer
    display->pixels = malloc(width * height * sizeof(uint16_t));
    if (!display->pixels) {
        printf("Failed to allocate pixel buffer\n");
        SDL_DestroyTexture(display->texture);
        free(display);
        return NULL;
    }

    // Load font
    display->font = font;
    if (!display->font) {
        printf("Failed to load font\n");
        exit(1);
    }
    
    // Force fixed 8x8 character size to match tile system
    display->char_width = 8;
    display->char_height = 8;

    clear_screen_buffer(display, 0x0000);
    display->frames_rendered = 0;

    return display;
}

void display_destroy(display_t* display) {
    if (display) {
        if (display->pixels) {
            free(display->pixels);
        }
        if (display->texture) {
            SDL_DestroyTexture(display->texture);
        }
        free(display);
    }
    TTF_Quit();
}

void display_update(display_t* display, SDL_FRect *where) {
    if (!display) return;
    
    // Clear screen with black background
    clear_screen_buffer(display, 0x0000);
    
    // Also render sprites if they exist
    render_sprites(display);

    render_text(display);

    // Update texture for rendering
    SDL_UpdateTexture(display->texture, NULL, display->pixels, display->width * sizeof(uint16_t));
    
    display->frames_rendered++;
}


void clear_screen_buffer(display_t* display, uint16_t color) {
    for (int i = 0; i < display->width * display->height; i++) {
        display->pixels[i] = color;
    }
}

uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b) {
    return ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);
}


void render_text(display_t* display) {
    if (!display->font) return;

    // Read text colors once per frame for performance
    uint16_t fg_color = memory_read_word(display->memory, VIDEO_CONTROL_START + 10);
    uint16_t bg_color = memory_read_word(display->memory, VIDEO_CONTROL_START + 12);
    
    // Use default colors if not set
    if (fg_color == 0 && bg_color == 0) {
        fg_color = 0xFFFF; // White
        bg_color = 0x0000; // Black
    }


    // Render text grid (40x30 characters) - read from character buffer
    for (int tile_y = 0; tile_y < SCREEN_HEIGHT_TILES; tile_y++) {
        for (int tile_x = 0; tile_x < SCREEN_WIDTH_TILES; tile_x++) {
            // Read character from buffer at 0xD000-0xD4AF
            uint16_t buffer_addr = CHAR_BUFFER_START + (tile_y * SCREEN_WIDTH_TILES + tile_x);
            uint8_t ch = memory_read_byte(display->memory, buffer_addr);
            
            // Skip rendering empty/invisible characters
            if (ch < 32 || ch > 126) continue;

            // Get 8x8 bitmap glyph from font
            const uint8_t* glyph = font8x8_basic[ch];
            
            // Position character at tile location
            int pixel_x = tile_x * 8;
            int pixel_y = tile_y * 8;
            
            // Render 8x8 bitmap directly to pixel buffer
            for (int row = 0; row < 8; row++) {
                uint8_t bits = glyph[row];
                for (int col = 0; col < 8; col++) {
                    // Check if bit is set (foreground) or clear (background)
                    uint16_t color = (bits & (1 << col)) ? fg_color : bg_color;
                    
                    int px = pixel_x + col;
                    int py = pixel_y + row;
                    if (px >= 0 && px < display->width && py >= 0 && py < display->height) {
                        display->pixels[py * display->width + px] = color;
                    }
                }
            }
        }
    }
}

// Sprite rendering functions
void render_sprites(display_t* display) {
    for (int i = 0; i < MAX_SPRITES; i++) {
        uint16_t sprite_addr = SPRITE_TABLE_START + (i * 3);
        uint8_t sprite_x = memory_read_byte(display->memory, sprite_addr + 0);
        uint8_t sprite_y = memory_read_byte(display->memory, sprite_addr + 1);
        uint8_t tile_id = memory_read_byte(display->memory, sprite_addr + 2);
        
        // Skip if sprite is disabled (tile_id = 0) or off-screen
        if (tile_id == 0 || tile_id >= MAX_TILES || sprite_x >= SCREEN_WIDTH_TILES || sprite_y >= SCREEN_HEIGHT_TILES) {
            continue;
        }
        
        // Convert tile coordinates to pixel coordinates
        render_sprite_to_buffer(display, tile_id, sprite_x * 8, sprite_y * 8);
    }
}

void render_sprite_to_buffer(display_t* display, uint8_t id, uint16_t x, uint16_t y) {
    // Calculate address of sprite pixel data (64 bytes per 8x8 sprite)
    // tile_id=1 refers to first tile at TILESET_DATA_START, so offset by (id-1)
    uint16_t sprite_data_addr = TILESET_DATA_START + ((id - 1) * 64);
    
    // Track previous valid palette index for this tile
    uint8_t prev_palette_index = 0; // Default to palette 0
    
    // Render 8x8 sprite using pixel data
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            // Get palette index for this pixel
            uint8_t palette_index = memory_read_byte(display->memory, sprite_data_addr + (py * 8 + px));
            
            // Use previous valid palette if index is 16+
            if (palette_index >= PALETTE_SIZE) {
                palette_index = prev_palette_index;
            } else if (palette_index > 0 && palette_index < PALETTE_SIZE) {
                prev_palette_index = palette_index; // Update previous valid index
            }
            int final_x = x + px;
            int final_y = y + py;
            
            if (final_x >= 0 && final_x < display->width &&
                final_y >= 0 && final_y < display->height) {
                uint16_t color = get_palette_color(display, palette_index);
                display->pixels[final_y * display->width + final_x] = color;
            }
        }
    }
}

uint16_t get_palette_color(display_t *display, uint8_t palette_index) {
    // Palette index should be 1-15 when this function is called
    if (palette_index == 0 || palette_index >= PALETTE_SIZE) palette_index = 1;
    uint16_t color_addr = PALETTE_RAM_START + (palette_index * 2);
    return memory_read_word(display->memory, color_addr);
}


