#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "cpu.h"
#include "tools/dasm.h"

#define SCREEN_WIDTH 32
#define SCREEN_HEIGHT 24
#define CHAR_WIDTH 8
#define CHAR_HEIGHT 8
#define SCALE 2

Cpu_t* cpu;

// Convert RGB565 to SDL_Color
SDL_Color rgb565_to_sdl_color(uint16_t color) {
    SDL_Color sdl_color;
    sdl_color.r = ((color >> 11) & 0x1F) << 3;  // 5 bits red
    sdl_color.g = ((color >> 5) & 0x3F) << 2;   // 6 bits green
    sdl_color.b = (color & 0x1F) << 3;          // 5 bits blue
    sdl_color.a = 255;
    return sdl_color;
}

void cpu_cycle() {
    printf("r3 = 0x%04X  -- r4 = 0x%04X\n", cpu->r[3], cpu->r[4]);
    uint16_t inst = fetch(cpu);
    printf("%u: %s\n",cpu->pc, disassemble_word(inst));
    execute(decode(inst), cpu);
}

/*
- x, y: upper left corner.
- texture, rect: outputs.
*/
void get_text_and_rect(SDL_Renderer *renderer, int x, int y, char *text,
        TTF_Font *font, SDL_Texture **texture, SDL_Rect *rect) {
    int text_width;
    int text_height;
    SDL_Surface *surface;
    SDL_Color textColor = {255, 255, 255, 0};

    surface = TTF_RenderText_Solid(font, text, textColor);
    *texture = SDL_CreateTextureFromSurface(renderer, surface);
    text_width = surface->w;
    text_height = surface->h;
    SDL_FreeSurface(surface);
    rect->x = x;
    rect->y = y;
    rect->w = text_width;
    rect->h = text_height;
}

int main(void) {
    // Create a cpu
    cpu = cpu_init();

    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return 1;
    }
    
    // Create window
    SDL_Window* window = SDL_CreateWindow(
        "IDN-16 Console", 
        SDL_WINDOWPOS_UNDEFINED, 
        SDL_WINDOWPOS_UNDEFINED, 
        SCREEN_WIDTH * CHAR_WIDTH * SCALE, 
        SCREEN_HEIGHT * CHAR_HEIGHT * SCALE, 
        SDL_WINDOW_SHOWN
    );
    
    if (!window) {
        printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }
    
    // Create renderer
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }
    
    // Set scale
    SDL_RenderSetScale(renderer, SCALE, SCALE);
    
    TTF_Init();
    TTF_Font *font = TTF_OpenFont("src/core/fonts/FreeMonoOblique.ttf", 24);
    if (font == NULL) {
        fprintf(stderr, "error: font not found\n");
        exit(EXIT_FAILURE);
    }
    SDL_Rect rect1;
    SDL_Texture *texture1;


    // Main loop
    bool quit = false;
    SDL_Event e;
    
    while (!quit && cpu->running) {
        // Handle events
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }
        memory_dump(cpu->memory, VIDEO_MEM_START, 4, 4);
        cpu_cycle();
        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer);
        
        // Read palette entry 0 (should be green from startup.asm)
        uint16_t palette_entry_0 = memory_read_word(cpu->memory, PALETTE_MEM_START);
        SDL_Color text_color = rgb565_to_sdl_color(palette_entry_0);
        // Render video memory
        // Get character from video memory
        
        uint8_t char_code = memory_read_byte(cpu->memory, VIDEO_MEM_START);
        // Skip empty/space characters
        if (char_code < 32) continue;
        char text[6];
        for (int i = 0; i < 6; i++) {
            char_code = memory_read_byte(cpu->memory, VIDEO_MEM_START+i);
            text[i] = char_code;
        }
        printf("%s", text);
        get_text_and_rect(renderer, 0, 0, text, font, &texture1, &rect1);
        // Set drawing color
        SDL_SetRenderDrawColor(renderer, text_color.r, text_color.g, text_color.b, 255);
        SDL_RenderClear(renderer);

        SDL_RenderCopy(renderer, texture1, NULL, &rect1);

        SDL_RenderPresent(renderer);
        
        // Update screen
        SDL_RenderPresent(renderer);
    }
    
    // Clean up
    cpu_destroy(cpu);
    SDL_DestroyTexture(texture1);
    TTF_Quit();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}