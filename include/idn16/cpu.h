#ifndef IDN16_CPU_H
#define IDN16_CPU_H

#define CPU_CLOCK_HZ 10000  // .5 MHz CPU
#define DISPLAY_REFRESH_HZ 60 // 60hz
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
void cpu_cycle(Cpu_t* cpu);

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

#endif // IDN16_CPU_H