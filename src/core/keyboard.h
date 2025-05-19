#ifndef IDN16_KEYBOARD_H
#define IDN16_KEYBOARD_H

#include <SDL2/SDL.h>
#include "stdbool.h"

/*
 * Initialize the keyboard state.
 * This function should be called once at the beginning of the program.
 */
void initialize_keyboard();


/*
 * Check if a specific key is pressed. 
 * If the keyboard state is not initialized, this function will call initialize_keyboard() to set it up.
 * @param scancode The SDL scancode of the key to check.
 * @return true if the key is pressed, false otherwise.
 */
bool is_key_pressed(SDL_Scancode scancode);

#endif // IDN16_KEYBOARD_H