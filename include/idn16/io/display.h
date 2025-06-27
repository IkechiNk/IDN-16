#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>
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
    
    // TTF font for text rendering
    TTF_Font* font;
    int char_width;
    int char_height;
    
    // Performance stats
    uint32_t frames_rendered;
} display_t;

/*
 * Initializes the display with the given width and height.
 * Returns a pointer to the display structure.
 */
display_t* display_init(int width, int height, int scale, uint8_t memory[], SDL_Renderer *renderer, TTF_Font *font);

/*
 * Destroys the display and frees the allocated resources.
 */
void display_destroy(display_t *display);

/*
 * Updates the screen with the newest pixel information.
 */
void display_update(display_t *display, SDL_FRect *where);

/*
 * Text rendering functions
 */
void render_text_mode(display_t* display);
void render_char_at_pixel(display_t* display, uint8_t ch, int pixel_x, int pixel_y, uint16_t fg, uint16_t bg);

/*
 * Sprite rendering functions
 */
void render_sprites(display_t* display);
void render_sprite_to_buffer(display_t* display, uint8_t id, uint16_t x, uint16_t y);
uint16_t get_palette_color(display_t* display, uint8_t palette_index);

/*
 * Utility functions
 */
void clear_screen_buffer(display_t* display, uint16_t color);
uint16_t rgb_to_rgb565(uint8_t r, uint8_t g, uint8_t b);


#endif // DISPLAY_H