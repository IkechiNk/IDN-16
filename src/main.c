#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include "idn16/io/display.h"
#include "idn16/io/keyboard.h"
#include "idn16/cpu.h"
#include "../lib/sfd/sfd.h"

#define CLAY_IMPLEMENTATION
#include "../lib/clay/clay.h"
#include "../lib/clay/SDL3/clay_renderer_SDL3.c"

#define COLOR_LIGHT (Clay_Color){224, 215, 210, 255}
#define COLOR_ORANGE (Clay_Color){225, 138, 50, 255}
#define COLOR_RED (Clay_Color){205, 92, 92, 255}
#define COLOR_GREEN (Clay_Color){142, 177, 92, 255}
#define COLOR_BLUE (Clay_Color){111, 173, 162, 255}
#define COLOR_BLACK (Clay_Color){0, 0, 0, 255}
#define COLOR_WHITE (Clay_Color){255, 255, 255, 255}
#define COLOR_DARK (Clay_Color){100, 100, 100, 255}
#define COLOR_HOVER (Clay_Color){0, 0, 0, 25}
#define COLOR_TRANSPARENT (Clay_Color){0, 0, 0, 0}

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;
display_t *display = NULL;
Cpu_t* cpu = NULL;
TTF_TextEngine *text_engine = NULL;
Clay_SDL3RendererData *renderer_data;

SDL_FRect* emulator_rect = NULL;

TTF_Font **fonts;
static const uint32_t FONT_ID = 0;

const float ScreenWidth = 1408.0f;
const float ScreenHeight = 792.0f;
void *clay_mem;

bool debug_mode = false;
static int visible_menu = -1;

static FILE* loaded_rom_file = NULL;

// Utility to detect WSL
static bool is_wsl() {
    FILE *f = fopen("/proc/version", "r");
    if (!f) return false;
    char buf[256] = {0};
    fread(buf, 1, sizeof(buf)-1, f);
    fclose(f);
    return strstr(buf, "Microsoft") != NULL || strstr(buf, "WSL") != NULL;
}

/* Clay layout definitions */
Clay_Sizing Layout_Expand = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_GROW(0)
};
Clay_Sizing Layout_Header = {
    .width = CLAY_SIZING_GROW(0),
};
Clay_Sizing Layout_Menu_Item = {
    .width = CLAY_SIZING_GROW(0),
    .height = CLAY_SIZING_FIXED(40)
};
Clay_Sizing Layout_Registers = {
    .width = CLAY_SIZING_FIXED(200),
    .height = CLAY_SIZING_GROW(0)
};
Clay_Sizing Layout_Assembly = {
    .width = CLAY_SIZING_FIXED(200),
    .height = CLAY_SIZING_GROW(0)
};

static inline Clay_Dimensions SDL_MeasureText(Clay_StringSlice text, Clay_TextElementConfig *config, void *user_data) {
    TTF_Font **fonts = user_data;
    TTF_Font *font = fonts[config->fontId];
    int width, height;

    if (!TTF_GetStringSize(font, text.chars, text.length, &width, &height)) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to measure text: %s", SDL_GetError());
    }

    return (Clay_Dimensions) { (float) width, (float) height };
}

void Handle_Clay_Errors(Clay_ErrorData error_data) {
    printf("%s\n", error_data.errorText.chars);
}

// Menu action handler functions
void file_open_rom() { 
    printf("Opening ROM...\n");
    sfd_Options opt = {
        .title = "Open ROM",
        .filter_name = "IDN-16 ROMs",
        .filter = "*.bin|*.rom"
    };
    const char *filename = sfd_open_dialog(&opt);
    if (filename) {
        printf("Got file: '%s'\n", filename);
        loaded_rom_file = fopen(filename, "r");
    } else {
        printf("Open canceled\n");
    }
}
void file_close_rom() { 
    if (loaded_rom_file) {
        fclose(loaded_rom_file);
        loaded_rom_file = NULL;
    }
    printf("Closing ROM...\n"); 
}
void file_exit() { cpu->running = false; }

void view_debug_mode() { debug_mode = !debug_mode; printf("Debug mode: %s\n", debug_mode ? "ON" : "OFF"); }
void view_memory_viewer() { if (debug_mode) memory_dump(cpu->memory, 0x0000, 64, 16); }
void view_cpu_registers() { printf("CPU Registers view\n"); }
void view_display_settings() { printf("Display Settings\n"); }
void view_assembly_listing() { printf("Assembly Listing\n"); }
void view_symbol_table() { printf("Symbol Table\n"); }

void run_start_resume() { printf("Start/Resume\n"); }
void run_pause() { printf("Pause\n"); }
void run_step_instruction() { printf("Step Instruction\n"); }
void run_reset_cpu() { printf("Reset CPU\n"); }
void run_load_system_rom() { printf("System ROM no longer needed - using C function simulation\n"); }

void tools_assembler() { printf("Assembler\n"); }
void tools_disassembler() { printf("Disassembler\n"); }
void tools_memory_editor() { printf("Memory Editor\n"); }
void tools_rom_manager() { printf("ROM Manager\n"); }

void debug_breakpoints() { printf("Breakpoints\n"); }
void debug_call_stack() { printf("Call Stack\n"); }
void debug_memory_dump() { printf("Memory Dump\n"); }
void debug_io_monitor() { printf("I/O Monitor\n"); }

void options_emulation_speed() { printf("Emulation Speed\n"); }
void options_audio_settings() { printf("Audio Settings\n"); }
void options_graphics_settings() { printf("Graphics Settings\n"); }
void options_input_configuration() { printf("Input Configuration\n"); }
void options_preferences() { printf("Preferences\n"); }

typedef void (*MenuAction)(void);

MenuAction file_actions[] = { file_open_rom, file_close_rom, file_exit };
MenuAction view_actions[] = { view_debug_mode, view_memory_viewer, view_cpu_registers, view_display_settings, view_assembly_listing, view_symbol_table };
MenuAction run_actions[] = { run_start_resume, run_pause, run_step_instruction, run_reset_cpu, run_load_system_rom };
MenuAction tools_actions[] = { tools_assembler, tools_disassembler, tools_memory_editor, tools_rom_manager };
MenuAction debug_actions[] = { debug_breakpoints, debug_call_stack, debug_memory_dump, debug_io_monitor };
MenuAction options_actions[] = { options_emulation_speed, options_audio_settings, options_graphics_settings, options_input_configuration, options_preferences };

MenuAction* menu_action_arrays[] = { file_actions, view_actions, run_actions, tools_actions, debug_actions, options_actions };

void Header_Menu_Item_Click(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) {
        MenuAction action = (MenuAction)userData;
        if (action) action();
        visible_menu = -1;
    }
}
void Header_Menu_Item(Clay_String text, MenuAction action) {
    CLAY((Clay_ElementDeclaration) {
        .layout = { .padding = {16, 8}, .sizing = Layout_Menu_Item},
        .backgroundColor = Clay_Hovered()? COLOR_HOVER : COLOR_TRANSPARENT,
        .cornerRadius = CLAY_CORNER_RADIUS(10) 
    }) {
        Clay_OnHover(Header_Menu_Item_Click, (intptr_t)action);
        CLAY_TEXT(text, CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 10, .textColor = COLOR_LIGHT}));
    };
}
void Header_Button_Hovered(Clay_ElementId elementId, Clay_PointerData pointerInfo, intptr_t userData) {
    int idx = (intptr_t)userData;
    if (pointerInfo.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME) visible_menu = visible_menu == idx? -1 : idx;
}
void Header_Button(Clay_String button_id, Clay_String buttonText, Clay_String menu_id, Clay_String *items[], int idx) {
    Clay_ElementId btn_id = CLAY_SID(button_id);
    Clay_ElementId mnu_id = CLAY_SID(menu_id);

    // Button
    CLAY((Clay_ElementDeclaration) {
        .id = btn_id,
        .layout = { .padding = {16, 16, 8, 8} },
        .backgroundColor = Clay_Hovered() || visible_menu == idx? COLOR_HOVER : COLOR_TRANSPARENT,
        .cornerRadius = CLAY_CORNER_RADIUS(10)
    }) {
        Clay_OnHover(Header_Button_Hovered, (intptr_t)idx);
        CLAY_TEXT(buttonText, CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 10, .textColor = COLOR_LIGHT }));

        // Show dropdown only if this menu is visible
        if (visible_menu == idx && items && items[0]) {
            CLAY((Clay_ElementDeclaration) {
                .id = mnu_id,
                .floating = (Clay_FloatingElementConfig) {
                    .attachTo = CLAY_ATTACH_TO_PARENT,
                    .attachPoints = {.element = CLAY_ATTACH_POINT_LEFT_TOP, .parent = CLAY_ATTACH_POINT_LEFT_BOTTOM},
                    .offset = {.y = 3},
                },
                .layout = {
                    .padding = { 2, 2, 4, 4 },
                    .layoutDirection = CLAY_TOP_TO_BOTTOM,
                    .sizing = {.width = 200},
                    .childGap = 4
                },
                .cornerRadius = CLAY_CORNER_RADIUS(5),
                .backgroundColor = COLOR_GREEN,
                .border = { .color = COLOR_BLACK, .width = {1, 1, 1, 1, 0} },
            }) {
                MenuAction* actions = menu_action_arrays[idx];
                for (int i = 0; items[i] != NULL; i++) {
                    Header_Menu_Item(*items[i], actions[i]);
                }
            };
        }
    };

    // Outside of the button/menu, close the dropdown on click
    bool pointer_over_btn = Clay_PointerOver(btn_id);
    bool pointer_over_menu = Clay_PointerOver(mnu_id);
    Clay_PointerData pointer = Clay_GetCurrentContext()->pointerInfo;
    if (visible_menu == idx && pointer.state == CLAY_POINTER_DATA_PRESSED_THIS_FRAME
        && !pointer_over_btn && !pointer_over_menu) {
        visible_menu = -1;
    }
}
// File menu items
Clay_String *file_menu_items[] = {
    &CLAY_STRING("Open ROM"),
    &CLAY_STRING("Close ROM"),
    &CLAY_STRING("Exit"),
    NULL
};
// View menu items
Clay_String *view_menu_items[] = {
    &CLAY_STRING("Debug Mode (F1)"),
    &CLAY_STRING("Memory Viewer (F4)"),
    &CLAY_STRING("CPU Registers"),
    &CLAY_STRING("Display Settings"),
    &CLAY_STRING("Assembly Listing"),
    &CLAY_STRING("Symbol Table"),
    NULL
};
// Run menu items
Clay_String *run_menu_items[] = {
    &CLAY_STRING("Start/Resume"),
    &CLAY_STRING("Pause"),
    &CLAY_STRING("Step Instruction"),
    &CLAY_STRING("Reset CPU"),
    &CLAY_STRING("Load System ROM"),
    NULL
};
// Tools menu items
Clay_String *tools_menu_items[] = {
    &CLAY_STRING("Assembler"),
    &CLAY_STRING("Disassembler"),
    &CLAY_STRING("Memory Editor"),
    &CLAY_STRING("ROM Manager"),
    NULL
};
// Debug menu items
Clay_String *debug_menu_items[] = {
    &CLAY_STRING("Breakpoints"),
    &CLAY_STRING("Call Stack"),
    &CLAY_STRING("Memory Dump"),
    &CLAY_STRING("I/O Monitor"),
    NULL
};
// Options menu items
Clay_String *options_menu_items[] = {
    &CLAY_STRING("Emulation Speed"),
    &CLAY_STRING("Audio Settings"),
    &CLAY_STRING("Graphics Settings"),
    &CLAY_STRING("Input Configuration"),
    &CLAY_STRING("Preferences"),
    NULL
};

Clay_RenderCommandArray App_Create_Layout() {
    Clay_BeginLayout();
    Clay_ElementDeclaration main_section = { .id = CLAY_ID("Main"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = Layout_Expand, .padding = CLAY_PADDING_ALL(0), .childGap = 16}, .backgroundColor = COLOR_DARK };
    Clay_ElementDeclaration header_section = { .id = CLAY_ID("Header"), .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = Layout_Header, .padding = { 16, 16, 3, 2 }, .childGap = 16, .childAlignment = { .y = CLAY_ALIGN_Y_CENTER }}, .backgroundColor = COLOR_GREEN };
    Clay_ElementDeclaration center = { .id = CLAY_ID("Center"), .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .sizing = Layout_Expand, .padding = { 0, 0, 0, 12}, .childGap = 16, .childAlignment = {CLAY_ALIGN_X_CENTER, CLAY_ALIGN_Y_CENTER} }, .backgroundColor = COLOR_DARK };
    Clay_ElementDeclaration emulator_section_decl = { 
        .id = CLAY_ID("Emulator"), 
        .layout = { .layoutDirection = CLAY_LEFT_TO_RIGHT, .padding = { 0, 0, 0, 0 }, .sizing = Layout_Expand}, 
        .backgroundColor = COLOR_BLACK, 
        .image = { .imageData = display ? display->texture : NULL },
        .aspectRatio = { .aspectRatio = 320/240}
    };
    Clay_ElementDeclaration register_section = { .id = CLAY_ID("Registers"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = Layout_Registers, .padding = { 8, 8, 8, 8 }, .childGap = 8 }, .backgroundColor = COLOR_DARK };
    Clay_ElementDeclaration assembly_section = { .id = CLAY_ID("Assembly"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = Layout_Assembly, .padding = { 8, 8, 8, 8 }, .childGap = 8 }, .backgroundColor = COLOR_DARK };

    CLAY(main_section) {
        CLAY(header_section) {
            Header_Button(CLAY_STRING("File_but"), CLAY_STRING("File"), CLAY_STRING("File_menu"), file_menu_items, 0);
            Header_Button(CLAY_STRING("View_but"), CLAY_STRING("View"), CLAY_STRING("View_menu"), view_menu_items, 1);
            Header_Button(CLAY_STRING("Run_but"), CLAY_STRING("Run"), CLAY_STRING("Run_menu"), run_menu_items, 2);
            Header_Button(CLAY_STRING("Tools_but"), CLAY_STRING("Tools"), CLAY_STRING("Tools_menu"), tools_menu_items, 3);
            Header_Button(CLAY_STRING("Debug_but"), CLAY_STRING("Debug"), CLAY_STRING("Debug_menu"), debug_menu_items, 4);
            Header_Button(CLAY_STRING("Options_but"), CLAY_STRING("Options"), CLAY_STRING("Options_menu"), options_menu_items, 5);
        };
        CLAY(center) {
            CLAY(emulator_section_decl);
            CLAY(register_section) {
                CLAY_TEXT(CLAY_STRING("Registers"), CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 16, .textColor = COLOR_LIGHT }));
                for (int i = 0; i < 8; i++) {
                    char reg[16];
                    int len = sprintf(reg, "r%d: 0x%04X", i, cpu->r[i]);
                    Clay_String clay_reg = {.isStaticallyAllocated = false, .length = len, .chars = reg};
                    CLAY_TEXT(clay_reg, CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 12, .textColor = COLOR_LIGHT }));
                };
            };
        };
    };
    return Clay_EndLayout();
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
    if (is_wsl()) {
        SDL_SetHint(SDL_HINT_RENDER_DRIVER, "software");
        printf("WSL detected: Forcing SDL software renderer\n");
    }
    
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
    int width, height;
    SDL_GetWindowSize(window, &width, &height);
    Clay_Dimensions dimensions = { width, height};
    Clay_Initialize(arena, dimensions, (Clay_ErrorHandler){Handle_Clay_Errors});
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
        case SDL_EVENT_MOUSE_BUTTON_UP:
            Clay_SetPointerState((Clay_Vector2) { event->button.x, event->button.y }, event->button.button == SDL_BUTTON_LEFT);
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
    Clay_RenderCommandArray render_commands = App_Create_Layout();

    SDL_Clay_RenderClayCommands(renderer_data, &render_commands);
    
    display_update(display, NULL);

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