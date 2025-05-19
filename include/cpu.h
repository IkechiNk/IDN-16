#include "include/memory.h"

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
        uint8_t i : 1;  // Interrupt enable flag
        uint8_t reserved : 3; // For future use
    } flags;

    // Cycle counter for CPU timing
    uint32_t cycles;

    // CPU state
    bool running; 
} Cpu_T;

/* Shared between CPU stages */
typedef struct
{
  uint16_t inst;
  uint8_t first;
  uint8_t second;
  uint8_t third;
} shared;

/*
 * Initializes the CPU.
 * Returns a pointer to the CPU struct.
 */
Cpu_T* cpu_init(void);

/*
 * Returns the current instruction according to the
 * cpu's program counter.
 */
uint16_t fetch();

/*
 * Returns the results of the decode stage.
 * The shared struct returned by decode is passed as
 * the input to execute.
 */
shared decode(uint16_t instruction);

/*
 * Executes the decoded instruction.
 */
void execute(shared info);
