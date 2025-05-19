#include "keyboard.h"

const Uint8 *keyboard_state; 


void initialize_keyboard() {
    keyboard_state = SDL_GetKeyboardState(NULL);
}


bool is_key_pressed(SDL_Scancode scancode) {
    if (keyboard_state == NULL) {
        initialize_keyboard();
    }
    return keyboard_state[scancode];
}
