#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL_main.h>

#include <SDL3/SDL.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <stdio.h>
#include "idn16/io/display.h"
#include "idn16/io/keyboard.h"
#include "idn16/cpu.h"
#include "idn16/dasm.h"
#include "idn16/asmblr.h"
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

bool display_registers = false;
bool display_assembly = false;
bool cycling = false;
static int visible_menu = -1;

static FILE* loaded_rom_file = NULL;
static uint64_t sleep_from = 0;

// Persistent buffers for register display
static char register_text_buffers[8][32];
static Clay_String register_strings[8];

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
    .width = CLAY_SIZING_FIXED(650),
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
void run_reset_cpu();
void file_close_rom() { 
    if (loaded_rom_file) {
        fclose(loaded_rom_file);
        loaded_rom_file = NULL;
    }
    run_reset_cpu();
    printf("Closing ROM...\n"); 
}
void file_open_rom() { 
    printf("Opening ROM...\n");
    sfd_Options opt = {
        .title = "Open ROM",
        .filter_name = "IDN-16 ROMs",
        .filter = "*.bin|*.rom"
    };
    const char *filename = sfd_open_dialog(&opt);
    if (filename) {
        file_close_rom();
        printf("Got file: '%s'\n", filename);
        loaded_rom_file = fopen(filename, "r");
        load_user_rom(cpu->memory, loaded_rom_file);
    } else {
        printf("Open canceled\n");
    }
}
void file_exit() { cpu->running = false; }

void view_memory_viewer() { memory_dump(cpu->memory, 0x0000, 64, 16); }
void view_cpu_registers() { display_registers = !display_registers; }
void view_assembly_listing() { display_assembly = !display_assembly; }

void run_start_resume() { if (loaded_rom_file) cycling = true; }
void run_pause() { cycling = false;}
void run_step_instruction() { 
    if (loaded_rom_file) {
        cycling = false; 
        cpu_cycle(cpu);
    }
}
void run_reset_cpu() { 
    cycling = false;
    cpu_destroy(cpu);
    display_destroy(display);
    
    /* Initialize the cpu */
    cpu = cpu_init();
    if (!cpu) {
        fprintf(stderr, "Unable to intialize cpu\n");
        exit(1);
    }
    /* Initialize the display */
    display = display_init(320, 240, 2, cpu->memory, renderer, fonts[0]);
    if (!display) {
        fprintf(stderr, "Couldn't load display\n");
        exit(1);
    }
    if (loaded_rom_file) load_user_rom(cpu->memory, loaded_rom_file);

}

void tools_assembler() { 
    sfd_Options opt_in = {
        .title = "Select assembly file to assemble",
        .filter_name = "IDN-16 ASMs",
        .filter = "*.asm|*.idn16"
    };
    const char *input_pre_copy = sfd_open_dialog(&opt_in);
    if (!input_pre_copy) {
        printf("No input selected\n");
        return;
    }

    // Make a copy of input before calling sfd_save_dialog
    char *input = malloc(strlen(input_pre_copy) + 1);
    strcpy(input, input_pre_copy);

    sfd_Options opt_out = {
        .title = "Choose where to save assembled rom",
        .filter_name = "IDN-16 ROMs",
        .filter = "*.rom|*.bin"
    };
    const char *output = sfd_save_dialog(&opt_out);
    if (!output) {
        printf("No output selected\n");
        free(input);
        return;
    }
    
    internal_assemble(input, output);
    free(input);
 }
void tools_disassembler() {
    sfd_Options opt_in = {
        .title = "Select rom file to disassemble",
        .filter_name = "IDN-16 ROMs",
        .filter = "*.rom|*.bin"
    };
    const char *input_pre_copy = sfd_open_dialog(&opt_in);
    if (!input_pre_copy) {
        printf("No input selected\n");
        return;
    }

    // Make a copy of input before calling sfd_save_dialog
    char *input = malloc(strlen(input_pre_copy) + 1);
    strcpy(input, input_pre_copy);

    sfd_Options opt_out = {
        .title = "Select where to save disassembled file",
        .filter_name = "File",
        .filter = "*.*"
    };
    const char *output = sfd_save_dialog(&opt_out);
    if (!output) {
        printf("No output selected\n");
        free(input);
        return;
    }
    
    internal_disassemble(input, output);
    free(input);
 }
void tools_memory_dump() { printf("Memory Editor\n"); }

typedef void (*MenuAction)(void);

MenuAction file_actions[] = { file_open_rom, file_close_rom, file_exit };
MenuAction view_actions[] = { view_memory_viewer, view_cpu_registers, view_assembly_listing };
MenuAction run_actions[] = { run_start_resume, run_pause, run_step_instruction, run_reset_cpu };
MenuAction tools_actions[] = { tools_assembler, tools_disassembler, tools_memory_dump };

MenuAction* menu_action_arrays[] = { file_actions, view_actions, run_actions, tools_actions };

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
    &CLAY_STRING("Memory Viewer (F1)"),
    &CLAY_STRING("CPU Registers"),
    &CLAY_STRING("Assembly Listing"),
    NULL
};
// Run menu items
Clay_String *run_menu_items[] = {
    &CLAY_STRING("Start/Resume"),
    &CLAY_STRING("Pause"),
    &CLAY_STRING("Step Instruction"),
    &CLAY_STRING("Reset CPU"),
    NULL
};
// Tools menu items
Clay_String *tools_menu_items[] = {
    &CLAY_STRING("Assembler"),
    &CLAY_STRING("Disassembler"),
    &CLAY_STRING("Memory Dump"),
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
    Clay_ElementDeclaration assembly_section = { .id = CLAY_ID("Assembly"), .layout = { .layoutDirection = CLAY_TOP_TO_BOTTOM, .sizing = Layout_Assembly, .padding = { 8, 8, 8, 0 }, .childGap = 8 }, .backgroundColor = COLOR_DARK };

    CLAY(main_section) {
        CLAY(header_section) {
            Header_Button(CLAY_STRING("File_but"), CLAY_STRING("File"), CLAY_STRING("File_menu"), file_menu_items, 0);
            Header_Button(CLAY_STRING("View_but"), CLAY_STRING("View"), CLAY_STRING("View_menu"), view_menu_items, 1);
            Header_Button(CLAY_STRING("Run_but"), CLAY_STRING("Run"), CLAY_STRING("Run_menu"), run_menu_items, 2);
            Header_Button(CLAY_STRING("Tools_but"), CLAY_STRING("Tools"), CLAY_STRING("Tools_menu"), tools_menu_items, 3);
        };
        CLAY(center) {
            CLAY(emulator_section_decl);
            if (display_registers) {
                CLAY(register_section) {
                    CLAY_TEXT(CLAY_STRING("Registers"), CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 16, .textColor = COLOR_LIGHT }));
                    
                    // Update register text buffers with current CPU state
                    for (int i = 0; i < 8; i++) {
                        int len = sprintf(register_text_buffers[i], "r%d: 0x%04X", i, cpu->r[i]);
                        register_strings[i] = (Clay_String){
                            .isStaticallyAllocated = false, 
                            .length = len, 
                            .chars = register_text_buffers[i]
                        };
                        CLAY_TEXT(register_strings[i], CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 12, .textColor = COLOR_LIGHT }));
                    }
                    
                    // Add PC and flags display
                    static char pc_buffer[32];
                    static char flags_buffer[64];
                    
                    int pc_len = sprintf(pc_buffer, "PC: 0x%04X", cpu->pc);
                    Clay_String pc_string = {.isStaticallyAllocated = false, .length = pc_len, .chars = pc_buffer};
                    CLAY_TEXT(pc_string, CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 12, .textColor = COLOR_ORANGE }));
                    
                    int flags_len = sprintf(flags_buffer, "Flags: Z:%d N:%d C:%d V:%d", 
                                        cpu->flags.z, cpu->flags.n, cpu->flags.c, cpu->flags.v);
                    Clay_String flags_string = {.isStaticallyAllocated = false, .length = flags_len, .chars = flags_buffer};
                    CLAY_TEXT(flags_string, CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 12, .textColor = COLOR_BLUE }));
                };
            }
            if (display_assembly) {
                CLAY(assembly_section) {
                    CLAY_TEXT(CLAY_STRING("Assembly"), CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 16, .textColor = COLOR_LIGHT }));
                    
                    // Show current instruction and some context, avoiding negative addresses
                    static char inst_buffers[5][80];
                    static Clay_String inst_strings[5];
                    int buffer_index = 0;
                    
                    // Calculate how many previous instructions we can safely show
                    int prev_count = (cpu->pc >= 4) ? 2 : (cpu->pc / 2);
                    int start_offset = -prev_count;
                    
                    for (int i = start_offset; i <= 2; i++) {
                        uint16_t addr = cpu->pc + (i * 2);
                        
                        // Skip if address would be negative
                        if ((int)cpu->pc + (i * 2) < 0) continue;
                        
                        uint16_t instruction = memory_read_word(cpu->memory, addr);
                        bool is_current = (i == 0);
                        
                        int len = sprintf(inst_buffers[buffer_index], "%s%04X: %s", 
                                        is_current ? "> " : " ", 
                                        addr, 
                                        disassemble_word(instruction));
                        inst_strings[buffer_index] = (Clay_String){
                            .isStaticallyAllocated = false, 
                            .length = len, 
                            .chars = inst_buffers[buffer_index]
                        };
                        
                        Clay_Color text_color = is_current ? COLOR_ORANGE : COLOR_LIGHT;
                        CLAY_TEXT(inst_strings[buffer_index], CLAY_TEXT_CONFIG({ .fontId = FONT_ID, .fontSize = 10, .textColor = text_color }));
                        
                        buffer_index++;
                    }
                };
            }
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
        SDL_Log("Failed to create text engine from renderer: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    fonts = SDL_calloc(1, sizeof(TTF_Font *));
    if (!fonts) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to allocate memory for the font array: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    TTF_Font *font = TTF_OpenFont("../resources/fonts/PixelifySans.ttf", 24);
    if (!font) {
        SDL_Log("Failed to load font: %s\n", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    fonts[FONT_ID] = font;

    /* Initialize the cpu */
    cpu = cpu_init();
    if (!cpu) {
        SDL_Log("Unable to intialize cpu\n");
        return SDL_APP_FAILURE;
    }
    /* Initialize the display */
    display = display_init(320, 240, 2, cpu->memory, renderer, font);
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

    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
    switch (event->type) {
        case SDL_EVENT_QUIT:
            cpu->running = false;
            break;
        case SDL_EVENT_KEY_DOWN:
            key_handler(display, event);
            break;
        case SDL_EVENT_KEY_UP:
            key_handler(display, event);
            if (event->key.key == SDLK_ESCAPE) {
                cpu->running = false;
            } else if (event->key.key == SDLK_F1) {
                memory_dump(cpu->memory, 0x0000, 64, 16);
                printf("Dumping memory\n");
            } else if (event->key.key == SDLK_SPACE) {
                run_step_instruction();
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


    for (int i = 0; (i < CYCLES_PER_FRAME) && cycling && cpu->running && cpu->sleep_timer == 0; i++) {
        cpu_cycle(cpu);

    }
    if (cpu->sleep_timer > 0 && sleep_from == 0) {
        sleep_from = SDL_GetTicks();
    }
    if (cpu->sleep_timer > 0 && sleep_from > 0) {
        uint64_t elapsed = SDL_GetTicks() - sleep_from;
        if (elapsed >= cpu->sleep_timer) {
            cpu->sleep_timer = 0;
            sleep_from = 0;
        }
    }

    // Increment frame counter every frame
    if (cycling && cpu->running) {
        cpu->frame_count++;
    }

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