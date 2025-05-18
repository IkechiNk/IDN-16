#include "include/memory.h"
#include "stdint.h"
#include "stdbool.h"

// Memory array
uint16_t memory[MEMORY_SIZE];
// Memory regions
static const MemoryRegion memory_regions[REGION_COUNT] = {
    {ROM_START, ROM_END, true, false, "ROM"},
    {RAM_START, RAM_END, false, false, "RAM"},
    {VIDEO_MEM_START, VIDEO_MEM_END, false, true, "Video Memory"},
    {SPRITE_MEM_START, SPRITE_MEM_END, false, true, "Sprite Memory"},
    {PALETTE_MEM_START, PALETTE_MEM_END, false, true, "Palette Memory"},
    {SOUND_REG_START, SOUND_REG_END, false, true, "Sound Registers"},
    {INPUT_REG_START, INPUT_REG_END, false, true, "Input Registers"},
    {SYSTEM_CTRL_START, SYSTEM_CTRL_END, false, true, "System Control Registers"},
    {SYSTEM_ROM_START, SYSTEM_ROM_END, true, false, "System ROM"},
};