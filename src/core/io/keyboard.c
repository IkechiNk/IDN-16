#include "idn16/io/keyboard.h"
#include "idn16/memory.h"

const bool *keys; 
SDL_Event e;

void key_handler(display_t* display, SDL_Event *e) {
    if (!keys) {
        keys = SDL_GetKeyboardState(NULL);
    }
    uint8_t cont1 = 0;
    uint8_t cont2 = 0;
    switch (e->type) {
        case SDL_EVENT_KEY_DOWN:
            cont1 |= (keys[SDL_SCANCODE_F]);
            cont1 |= (keys[SDL_SCANCODE_G] << 1);
            cont1 |= (keys[SDL_SCANCODE_TAB] << 2);
            cont1 |= (keys[SDL_SCANCODE_LSHIFT] << 3);
            cont1 |= (keys[SDL_SCANCODE_W] << 4);
            cont1 |= (keys[SDL_SCANCODE_S] << 5);
            cont1 |= (keys[SDL_SCANCODE_A] << 6);
            cont1 |= (keys[SDL_SCANCODE_D] << 7);

            cont2 |= (keys[SDL_SCANCODE_J]);
            cont2 |= (keys[SDL_SCANCODE_K] << 1);
            cont2 |= (keys[SDL_SCANCODE_RETURN] << 2);
            cont2 |= (keys[SDL_SCANCODE_RSHIFT] << 3);
            cont2 |= (keys[SDL_SCANCODE_UP] << 4);
            cont2 |= (keys[SDL_SCANCODE_DOWN] << 5);
            cont2 |= (keys[SDL_SCANCODE_LEFT] << 6);
            cont2 |= (keys[SDL_SCANCODE_RIGHT] << 7);
            break;
        case SDL_EVENT_KEY_UP:
            cont1 |= (keys[SDL_SCANCODE_F]);
            cont1 |= (keys[SDL_SCANCODE_G] << 1);
            cont1 |= (keys[SDL_SCANCODE_TAB] << 2);
            cont1 |= (keys[SDL_SCANCODE_LSHIFT] << 3);
            cont1 |= (keys[SDL_SCANCODE_W] << 4);
            cont1 |= (keys[SDL_SCANCODE_S] << 5);
            cont1 |= (keys[SDL_SCANCODE_A] << 6);
            cont1 |= (keys[SDL_SCANCODE_D] << 7);

            cont2 |= (keys[SDL_SCANCODE_J]);
            cont2 |= (keys[SDL_SCANCODE_K] << 1);
            cont2 |= (keys[SDL_SCANCODE_RETURN] << 2);
            cont2 |= (keys[SDL_SCANCODE_RSHIFT] << 3);
            cont2 |= (keys[SDL_SCANCODE_UP] << 4);
            cont2 |= (keys[SDL_SCANCODE_DOWN] << 5);
            cont2 |= (keys[SDL_SCANCODE_LEFT] << 6);
            cont2 |= (keys[SDL_SCANCODE_RIGHT] << 7);
            break;
        default:
            break;
    }
    memory_write_byte(display->memory, INPUT_CONTROLLER1, cont1, 1);
    memory_write_byte(display->memory, INPUT_CONTROLLER2, cont2, 1);
}
