#include "idn16/io/display.h"
#include "idn16/memory.h"
#include <string.h>

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
    
    // Always render text mode for system calls
    render_text_mode(display);
    
    // Also render sprites if they exist
    render_sprites(display);

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

// Draw a single character at specific pixel coordinates
void render_char_at_pixel(display_t* display, uint8_t ch, int pixel_x, int pixel_y, uint16_t fg, uint16_t bg) {
    if (!display || !display->font || ch < 32 || ch > 126) return;

    // Render the character to an SDL_Surface using TTF
    char text[2] = { ch, 0 };
    SDL_Color fg_color = { (fg >> 11) << 3, ((fg >> 5) & 0x3F) << 2, (fg & 0x1F) << 3, 255 };
    SDL_Color bg_color = { (bg >> 11) << 3, ((bg >> 5) & 0x3F) << 2, (bg & 0x1F) << 3, 255 };

    SDL_Surface* glyph = TTF_RenderText_Shaded(display->font, text, 1, fg_color, bg_color);
    if (!glyph) return;

    // Copy glyph pixels to display buffer at specified pixel coordinates (constrain to 8x8)
    int max_width = (glyph->w > 8) ? 8 : glyph->w;
    int max_height = (glyph->h > 8) ? 8 : glyph->h;
    for (int y = 0; y < max_height; y++) {
        for (int x = 0; x < max_width; x++) {
            uint32_t pixel = ((uint32_t*)glyph->pixels)[y * glyph->w + x];
            // Convert ARGB8888 to RGB565
            uint8_t r = (pixel >> 16) & 0xFF;
            uint8_t g = (pixel >> 8) & 0xFF;
            uint8_t b = (pixel) & 0xFF;
            uint16_t rgb565 = ((r & 0xF8) << 8) | ((g & 0xFC) << 3) | (b >> 3);

            int px = pixel_x + x;
            int py = pixel_y + y;
            if (px >= 0 && px < display->width && py >= 0 && py < display->height) {
                display->pixels[py * display->width + px] = rgb565;
            }
        }
    }

    SDL_DestroySurface(glyph);
}

void render_text_mode(display_t* display) {
    if (!display->font) return;

    // Read text colors once per frame for performance
    uint16_t fg_color = memory_read_word(display->memory, VIDEO_CONTROL_START + 10);
    uint16_t bg_color = memory_read_word(display->memory, VIDEO_CONTROL_START + 12);
    
    // Use default colors if not set
    if (fg_color == 0 && bg_color == 0) {
        fg_color = 0xFFFF; // White
        bg_color = 0x0000; // Black
    }

    // Render text grid (40x30 characters)
    for (int y = 0; y < 30; y++) {
        for (int x = 0; x < 40; x++) {
            uint16_t tile_addr = CHAR_BUFFER_START + (y * 40 + x);
            uint8_t ch = memory_read_byte(display->memory, tile_addr);
            
            // Skip rendering empty/invisible characters for performance
            if (ch >= 32 && ch <= 126) {
                int pixel_x = x * display->char_width;
                int pixel_y = y * display->char_height;
                render_char_at_pixel(display, ch, pixel_x, pixel_y, fg_color, bg_color);
            }
        }
    }
}

// Sprite rendering functions
void render_sprites(display_t* display) {
    for (int i = 0; i < 64; i++) {
        uint16_t sprite_addr = SPRITE_TABLE_START + (i * 3);
        uint8_t sprite_x = memory_read_byte(display->memory, sprite_addr + 0);
        uint8_t sprite_y = memory_read_byte(display->memory, sprite_addr + 1);
        uint8_t tile_id = memory_read_byte(display->memory, sprite_addr + 2);
        
        // Skip if sprite is disabled (tile_id = 0) or off-screen
        if (tile_id == 0 || sprite_x >= 40 || sprite_y >= 30) {
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
    uint8_t prev_palette_index = 1; // Default to palette 1
    
    // Render 8x8 sprite using pixel data
    for (int py = 0; py < 8; py++) {
        for (int px = 0; px < 8; px++) {
            // Get palette index for this pixel
            uint8_t palette_index = memory_read_byte(display->memory, sprite_data_addr + (py * 8 + px));
            
            // Skip transparent pixels (palette index 0)
            if (palette_index == 0) continue;
            
            // Use previous valid palette if index is 16+
            if (palette_index >= 16) {
                palette_index = prev_palette_index;
            } else if (palette_index >= 1 && palette_index <= 15) {
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
    if (palette_index < 1 || palette_index > 15) palette_index = 1;
    uint16_t color_addr = PALETTE_RAM_START + (palette_index * 2);
    return memory_read_word(display->memory, color_addr);
}


