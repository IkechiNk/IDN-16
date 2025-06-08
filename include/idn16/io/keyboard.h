#ifndef IDN16_KEYBOARD_H
#define IDN16_KEYBOARD_H

#include <SDL3/SDL.h>
#include "display.h"

/*
 * Handles keypresses and their corresponding actions
 */
void key_handler(display_t* display, SDL_Event *e);

#endif // IDN16_KEYBOARD_H