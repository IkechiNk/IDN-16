// #include <SDL3/SDL.h>
// #include <stdint.h>
// #include <stdbool.h>
// #include <stdio.h>
// #include "idn16/cpu.h"
// #include "idn16/io/display.h"
// #include "idn16/io/keyboard.h"

// // Interrupt types
// #define DISPLAY_INTERRUPT 0x1

// // Global system state
// Cpu_t* cpu;
// display_t* display;
// uint32_t frame_count = 0;
// bool debug_mode = false;

// void interrupt_check() {
//     uint8_t interrupt = memory_read_byte(cpu->memory, INTERRUPT_CONTROL_REG);
//     if (interrupt != 0) {
//         cpu->interrupt_pending = true;
//         cpu->interrupt_type = interrupt;
//         memory_write_byte(cpu->memory, INTERRUPT_CONTROL_REG, 0, true);
//     }
// }

// void interrupt_handler() {
//     switch (cpu->interrupt_type) {
//         case DISPLAY_INTERRUPT:
//             display_update(display);
//             frame_count++;
//             memory_write_word(cpu->memory, FRAME_COUNTER_LOW, frame_count & 0xFFFF, true);
//             break;
//         default:
//             break;
//     }
//     cpu->interrupt_pending = false;
//     cpu->interrupt_type = 0;
// }

// int main(int argc, char* argv[]) {
//     printf("IDN-16 Virtual Console\n");
    
//     // Parse command line arguments
//     const char* rom_file = NULL;
//     for (int i = 1; i < argc; i++) {
//         if (strcmp(argv[i], "--debug") == 0) {
//             debug_mode = true;
//         } else if (strcmp(argv[i], "--rom") == 0 && i + 1 < argc) {
//             rom_file = argv[++i];
//         } else if (strcmp(argv[i], "--help") == 0) {
//             printf("Usage: %s [options]\n", argv[0]);
//             printf("Options:\n");
//             printf("  --debug      Start in debug mode\n");
//             printf("  --rom FILE   Load ROM file\n");
//             printf("  --help       Show this help\n");
//             return 0;
//         }
//     }
//     // Initialize CPU
//     cpu = cpu_init();
//     if (!cpu) {
//         fprintf(stderr, "Failed to initialize CPU\n");
//         return 1;
//     }
//     if (!rom_file || !load_rom(cpu->memory, rom_file)) {
//         fprintf(stderr, "Error loading rom file: \"core/roms/%s\".\n", rom_file);
//         cpu_destroy(cpu);
//         return 1;
//     }
//     // Initialize display
//     display = display_init(320, 240, 2);
//     if (!display) {
//         fprintf(stderr, "Failed to initialize display\n");
//         cpu_destroy(cpu);
//         return 1;
//     }
    
//     printf("System ready. Press ESC to quit, F1 for debug.\n");
    
//     // Main loop
//     uint32_t last_frame_time = SDL_GetTicks();
    
//     while (cpu->running) {
//         uint32_t current_time = SDL_GetTicks();
        
//         // Handle SDL events
//         SDL_Event event;
//         while (SDL_PollEvent(&event)) {
//             switch (event.type) {
//                 case SDL_EVENT_QUIT:
//                     cpu->running = false;
//                     break;
//                 case SDL_EVENT_KEY_DOWN:
//                     if (event.key.key == SDLK_ESCAPE) {
//                         cpu->running = false;
//                     } else if (event.key.key == SDLK_F1) {
//                         debug_mode = !debug_mode;
//                         printf("Debug mode: %s\n", debug_mode ? "ON" : "OFF");
//                     } else if (event.key.key == SDLK_F4 && debug_mode) {
//                         memory_dump(cpu->memory, 0x0000, 64, 16);
//                     }
//                     break;
//             }
//         }
        
//         // Handle input
//         key_handler(display, cpu->memory);
        
//         // Execute some CPU cycles
//         for (int i = 0; i < 166; i++) { // ~10kHz at 60fps
//             if (cpu->interrupt_pending) {
//                 interrupt_handler();
//             }
//             cpu_cycle(cpu);
//             interrupt_check();
//             if (!cpu->running) break;
//         }
        
//         // Trigger display update
//         memory_write_byte(cpu->memory, INTERRUPT_CONTROL_REG, DISPLAY_INTERRUPT, true);
//         interrupt_check();
//         if (cpu->interrupt_pending) {
//             interrupt_handler();
//         }
        
//         // Debug info
//         if (debug_mode && current_time - last_frame_time >= 1000) {
//             printf("PC: 0x%04X, Flags: Z=%d N=%d C=%d V=%d\n", 
//                    cpu->pc, cpu->flags.z, cpu->flags.n, cpu->flags.c, cpu->flags.v);
//             last_frame_time = current_time;
//         }
        
//         // Frame limiting
//         uint32_t frame_time = SDL_GetTicks() - current_time;
//         if (frame_time < MS_PER_FRAME) {
//             SDL_Delay(MS_PER_FRAME - frame_time);
//         }
//     }
    
//     // Cleanup
//     printf("Shutting down...\n");
//     display_destroy(display);
//     cpu_destroy(cpu);
    
//     return 0;
// }