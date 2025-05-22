#ifndef DISPLAY_H
#define DISPLAY_H

#include <SDL2/SDL.h>
#include <stdio.h>

typedef struct  {
    int width;
    int height;
    SDL_Window *window;
    SDL_Surface *surface;
    SDL_Renderer *renderer;
} display_t;

/*
 * Initializes the display with the given width and height.
 * Returns a pointer to the display structure.
 * If initialization fails, returns NULL.
*/
display_t* display_init(int width, int height);

/*
 * Destroys the display and frees the allocated resources.
*/
void display_destroy(display_t *display);

/*
 * Updates the display surface.
*/
void display_update(display_t *display);

/*
 * Draws the given pixel data to the display.
 * The pixel data should be in the format expected by SDL.
*/
void display_draw(display_t *display, uint8_t *pixels, int width, int height);

#endif // DISPLAY_H