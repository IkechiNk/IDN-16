#ifndef IDN16_KEYBOARD_H
#define IDN16_KEYBOARD_H

#include <SDL2/SDL.h>
#include "display.h"

/*
 * Handles keypresses and their corresponding actions
 */
void key_handler(display_t* display, uint8_t memory[]);

#endif // IDN16_KEYBOARD_H