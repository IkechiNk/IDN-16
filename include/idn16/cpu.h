#ifndef IDN16_CPU_H
#define IDN16_CPU_H

#define CPU_CLOCK_HZ 1000000 // 1 MHz
#define DISPLAY_REFRESH_HZ 240 // 240 Hz
#define CYCLES_PER_FRAME (CPU_CLOCK_HZ / DISPLAY_REFRESH_HZ)
#define MS_PER_FRAME (int)(1000 / DISPLAY_REFRESH_HZ)

#include "memory.h"

typedef struct {
    uint16_t pc;
    /*
     * CPU register values.
     * r[6] is the stack pointer (SP). r[7] is the Link register (LR). 
     */
    uint16_t r[8];

    /*
     * System memory.
     */
    uint8_t memory[MEMORY_SIZE];
    
    // Flag values for flag reigster
    struct {
        uint8_t z : 1;  // Zero flag
        uint8_t n : 1;  // Negative flag
        uint8_t c : 1;  // Carry flag
        uint8_t v : 1;  // Overflow flag
        uint8_t reserved : 4; // For future use
    } flags;

    // Cycle counter for CPU timing
    uint64_t cycles;

    // Frame counter for display timing
    uint32_t frame_count;

    // Sleep timer
    uint16_t sleep_timer; // Timer for sleep functionality, in milliseconds
    
    uint32_t last_time; // Last time stamp for timing operations

    // Interrupt handling
    bool interrupt_pending;
    uint8_t interrupt_type;
    
    // CPU state
    bool running; 
} Cpu_t;

/* Shared between CPU stages */
typedef struct
{
  uint16_t inst;
  uint16_t first;
  uint8_t second;
  uint8_t third;
} shared;

/*
 * Initializes the CPU.
 * Returns a pointer to the CPU struct.
 */
Cpu_t* cpu_init(void);

/*
 * Destroys the CPU.
 * Frees the allocated memory pointed to by cpu.
 */
void cpu_destroy(Cpu_t* cpu);

/*
 * Runs a full fetch, decode, execute cycle on the cpu.
 */
void cpu_cycle(Cpu_t* cpu, bool debug);

/*
 * Returns the current instruction according to the
 * cpu's program counter.
 */
uint16_t fetch(Cpu_t* cpu);

/*
 * Returns the results of the decode stage.
 * The shared struct returned by decode is passed as
 * the input to execute.
 */
shared decode(uint16_t instruction);

/*
 * Executes the decoded instruction.
 */
void execute(shared info, Cpu_t* cpu);

/*
 * System call handler
 */
void handle_system_call(uint16_t address, Cpu_t* cpu);

/*
 * System call implementations (replace ROM functions)
 */
void syscall_clear_screen(Cpu_t* cpu);
void syscall_put_char(Cpu_t* cpu);
void syscall_put_string(Cpu_t* cpu);
void syscall_set_cursor(Cpu_t* cpu);
void syscall_get_cursor(Cpu_t* cpu);
void syscall_put_char_at(Cpu_t* cpu);
void syscall_scroll_up(Cpu_t* cpu);
void syscall_fill_area(Cpu_t* cpu);
void syscall_set_text_color(Cpu_t* cpu);
void syscall_get_input(Cpu_t* cpu);
void syscall_play_tone(Cpu_t* cpu);
void syscall_multiply(Cpu_t* cpu);
void syscall_divide(Cpu_t* cpu);  
void syscall_random(Cpu_t* cpu);
void syscall_memcpy(Cpu_t* cpu);
void syscall_print_hex(Cpu_t* cpu);
void syscall_print_dec(Cpu_t* cpu);
void syscall_set_sprite(Cpu_t* cpu);
void syscall_set_palette(Cpu_t* cpu);
void syscall_move_sprite(Cpu_t* cpu);
void syscall_set_sprite_pixel(Cpu_t* cpu);
void syscall_get_frame_count(Cpu_t* cpu);
void syscall_hide_sprite(Cpu_t* cpu);
void syscall_get_sprite_pos(Cpu_t* cpu);
void syscall_clear_sprite_range(Cpu_t* cpu);
void syscall_check_collision(Cpu_t* cpu);
void syscall_shift_sprites(Cpu_t* cpu);
void syscall_copy_sprite(Cpu_t* cpu);
void syscall_set_return_addr(Cpu_t* cpu);
void syscall_move_sprite_right(Cpu_t* cpu);
void syscall_move_sprite_left(Cpu_t* cpu);
void syscall_move_sprite_up(Cpu_t* cpu);
void syscall_move_sprite_down(Cpu_t* cpu);
void syscall_timer_start(Cpu_t* cpu);
void syscall_timer_stop(Cpu_t* cpu);
void syscall_timer_query(Cpu_t* cpu);
void syscall_sleep(Cpu_t* cpu);
void syscall_number_to_string(Cpu_t* cpu);

#endif // IDN16_CPU_H