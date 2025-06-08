#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>
#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include "idn16/io/display.h"
#include "idn16/io/keyboard.h"
#include "idn16/cpu.h"

#define CLAY_IMPLEMENTATION
#include "../lib/clay/clay.h"
#include "../lib/clay/SDL3/clay_renderer_SDL3.c"

#define COLOR_LIGHT (Clay_Color){224, 215, 210, 255}
#define COLOR_ORANGE (Clay_Color){225, 138, 50, 255}
#define COLOR_BLUE (Clay_Color){111, 173, 162, 255}
#define COLOR_BLACK (Clay_Color){0, 0, 0, 255}
#define COLOR_WHITE (Clay_Color){255, 255, 255, 255}
#define COLOR_DARK (Clay_Color){100, 100, 100, 255}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
display_t *display = NULL;
Cpu_t* cpu = NULL;
TTF_TextEngine *text_engine = NULL;
Clay_SDL3RendererData *renderer_data;

SDL_FRect* emulator_section = NULL;

TTF_Font **fonts;
static const uint32_t FONT_ID = 0;

const float ScreenWidth = 1408.0f;
const float ScreenHeight = 792.0f;
void *clay_mem;

bool debug_mode = false;

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *userData) {
    TTF_Font **fonts = userData;
    TTF_Font *font = fonts[config->fontId];
    int width, height;

    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions) { (float) width, (float) height };
}

void HandleClayErrors(Clay_ErrorData errorData) {
    printf("%s\n", errorData.errorText.chars);
}

Clay_RenderCommandArray App_CreateLayout() {
    Clay_BeginLayout();

    Clay_Sizing layoutExpand = {
        .width = CLAY_SIZING_GROW(0),
        .height = CLAY_SIZING_GROW(0)
    };

    CLAY((Clay_ElementDeclaration) {
        .id = CLAY_ID("Main"),
        .layout = {
            .layoutDirection = CLAY_TOP_TO_BOTTOM,
            .sizing = layoutExpand,
            .padding = CLAY_PADDING_ALL(16),
            .childGap = 16
        },
        .backgroundColor = COLOR_WHITE
    }) {
        CLAY_TEXT(
            CLAY_STRING("TEXT 1"), 
            &((Clay_TextElementConfig) {
                .fontId = FONT_ID,
                .textColor = COLOR_BLACK,
                .fontSize = 16
            })
        );
        CLAY_TEXT(
            CLAY_STRING("TEXT 2"), 
            &((Clay_TextElementConfig) {
                .fontId = FONT_ID,
                .textColor = COLOR_BLACK,
                .fontSize = 16
            })
        );
    }

    return Clay_EndLayout();
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("IDN-16 Virtual Console", ScreenWidth, ScreenHeight, 0, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetWindowResizable(window, true);
    
    if (!TTF_Init()) {
        SDL_Log("Couldn't initialize font system: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    
    text_engine = TTF_CreateRendererTextEngine(renderer);
    if (!text_engine) {
        SDL_Log("Failed to create text engine from renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    fonts = SDL_calloc(1, sizeof(TTF_Font *));
    if (!fonts) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for the font array: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    TTF_Font *font = TTF_OpenFont("resources/fonts/PixelifySans.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    fonts[FONT_ID] = font;

    /* Initialize the cpu */
    cpu = cpu_init();
    if (!cpu) {
        SDL_Log("Unable to intialize cpu");
        return SDL_APP_FAILURE;
    }
    /* Initialize the display */
    display = display_init(320, 240, 2, cpu->memory, renderer);
    if (!display) {
        SDL_Log("Couldn't load display\n");
        return SDL_APP_FAILURE;
    }

    /* Initialize Clay */
    uint32_t totalMemorySize = Clay_MinMemorySize();
    clay_mem = malloc(totalMemorySize);
    Clay_Arena arena = Clay_CreateArenaWithCapacityAndMemory(totalMemorySize, clay_mem);
    Clay_Dimensions dimensions = { .width = ScreenWidth, .height = ScreenHeight};
    Clay_Initialize(arena, dimensions, (Clay_ErrorHandler){HandleClayErrors});
    Clay_SetMeasureTextFunction(SDL_MeasureText, fonts);
    
    renderer_data = malloc(sizeof(Clay_SDL3RendererData));
    renderer_data->textEngine = text_engine;
    renderer_data->fonts = fonts;
    renderer_data->renderer = renderer;

    printf("Debug mode: %s\n", debug_mode ? "ON" : "OFF");
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    key_handler(display, event);

    switch (event->type) {
        case SDL_EVENT_QUIT:
            cpu->running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            if (event->key.key == SDLK_ESCAPE) {
                cpu->running = false;
            } else if (event->key.key == SDLK_F1) {
                debug_mode = !debug_mode;
                printf("Debug mode: %s\n", debug_mode ? "ON" : "OFF");
            } else if (event->key.key == SDLK_F4 && debug_mode) {
                memory_dump(cpu->memory, 0x0000, 64, 16);
                printf("Dumping memory\n");
            }
            break;
        case SDL_EVENT_WINDOW_RESIZED:
            Clay_SetLayoutDimensions((Clay_Dimensions) { (float) event->window.data1, (float) event->window.data2 });
            break;
        case SDL_EVENT_MOUSE_MOTION:
            Clay_SetPointerState((Clay_Vector2) { event->motion.x, event->motion.y },
                                 event->motion.state & SDL_BUTTON_LMASK);
            break;
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            Clay_SetPointerState((Clay_Vector2) { event->button.x, event->button.y },
                                 event->button.button == SDL_BUTTON_LEFT);
            break;
        case SDL_EVENT_MOUSE_WHEEL:
            Clay_UpdateScrollContainers(true, (Clay_Vector2) { event->wheel.x, event->wheel.y }, 0.01f);
            break;
    }
    return SDL_APP_CONTINUE;
}

/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
    if (!cpu->running) {
        return SDL_APP_SUCCESS;
    }

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    Clay_RenderCommandArray render_commands = App_CreateLayout();

    SDL_Clay_RenderClayCommands(renderer_data, &render_commands);
    
    // display_update(display, emulator_section);

    SDL_RenderPresent(renderer);

    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
    display_destroy(display);
    cpu_destroy(cpu);
    if (fonts) {
        for(size_t i = 0; i < sizeof(fonts) / sizeof(*fonts); i++) {
            TTF_CloseFont(fonts[i]);
        }
        SDL_free(fonts);
    }
    if (text_engine) TTF_DestroyRendererTextEngine(text_engine);
    if (clay_mem) free(clay_mem);
    if (renderer_data) free(renderer_data);
    TTF_Quit();
}