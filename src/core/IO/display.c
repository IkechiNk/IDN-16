#include "display.h"

display_t* display_init(int width, int height) {
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0) {
        printf("Failed to start the display\n");
        return NULL;
    }
    SDL_Window *window = SDL_CreateWindow("IDN-16", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, width, height, 0);
    if(!window) {
        printf("Failed to start the display\n");
        return NULL;
    }
    SDL_Surface *window_surface = SDL_GetWindowSurface(window);
    if(!window_surface) {
        printf("Failed to start the display\n");
        return NULL;
    }
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_UpdateWindowSurface(window);
    display_t *display = malloc(sizeof(display_t));
    display->width = width;
    display->height = height;
    display->window = window;
    display->surface = window_surface;
    display->renderer = renderer;
    return display;
}

void display_destroy(display_t *display) {
    if(display) {
        SDL_QuitSubSystem(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
        SDL_FreeSurface(display->surface);
        SDL_DestroyRenderer(display->renderer);
        SDL_DestroyWindow(display->window);
        SDL_Quit();
        free(display);
    }
}