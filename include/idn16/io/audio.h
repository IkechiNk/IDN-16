#ifndef IDN16_IO_AUDIO_H
#define IDN16_IO_AUDIO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>
#include "idn16/memory.h"

typedef struct {
    float frequency;
    float duration;
    float volume;
    bool enabled;
    float phase;
    float remaining_time;
} AudioChannel;

/*
 * Initialize the audio system
 * Returns true on success, false on failure
 */
bool audio_init(uint8_t *memory);

/*
 * Destroy the audio system and free resources
 */
void audio_destroy(void);

/*
 * Update audio channels from memory-mapped registers
 * Should be called regularly to sync with CPU memory
 */
void audio_update(uint8_t *memory);

/*
 * Enable or disable audio globally
 */
void audio_set_enabled(bool enabled);

/*
 * Get current audio enabled state
 */
bool audio_get_enabled(void);

#endif // IDN16_IO_AUDIO_H