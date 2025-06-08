#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct  {
    int width;
    int height;
    int scale;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    uint8_t* memory;
    uint16_t* pixels;
    
    // Tile cache for fast rendering
    uint32_t* tile_cache[256];
    bool tile_cache_dirty[256];
    
    // Performance stats
    uint32_t frames_rendered;
    uint32_t tiles_rendered;
    uint32_t sprites_rendered;
} display_t;

/*
 * Initializes the display with the given width and height.
 * Returns a pointer to the display structure.
 */
display_t* display_init(int width, int height, int scale, uint8_t memory[], SDL_Renderer *renderer);

/*
 * Destroys the display and frees the allocated resources.
 */
void display_destroy(display_t *display);

/*
 * Updates the screen with the newest pixel information.
 */
void display_update(display_t *display, SDL_FRect *where);

/*
 * Render based on current video mode
 */
void render_text_mode(display_t* display);
void render_tile_mode(display_t* display);
void render_mixed_mode(display_t* display);

/*
 * Helper functions
 */
void render_tile_to_buffer(display_t* display, int tile_id, int screen_x, int screen_y);
void render_sprite_to_buffer(display_t* display, int sprite_index);
void update_tile_cache(display_t* display, int tile_id);
uint16_t get_palette_color(display_t* display, int palette_index);

/*
 * Utility functions
 */
void clear_screen_buffer(display_t* display, uint16_t color);
uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b);

#endif // DISPLAY_H