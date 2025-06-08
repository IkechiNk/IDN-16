# `IDN-16` Console
## Table of Contents
<!-- TODO: WHEN GETTING "{lib}.dll" is not a .so shared library, this problem is becuase cmake is returning the windows path to the libary.-->
- [`IDN-16` Console](#idn-16-console)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installing](#installing)
  - [Tools](#tools)
    - [Assembler](#assembler)
    - [Disassembler](#disassembler)
    - [Example Programs](#example-programs)
  - [Documentation](#documentation)
    - [Memory Mapping](#memory-mapping)
      - [High-level Memory Layout](#high-level-memory-layout)
      - [Video Memory Layout (0x3000-0x3FFF)](#video-memory-layout-0x3000-0x3fff)
    - [IO-Interfacing](#io-interfacing)
      - [Audio System (0x4000-0x40FF)](#audio-system-0x4000-0x40ff)
      - [Input System (0x4100-0x41FF)](#input-system-0x4100-0x41ff)
      - [System Control (0x4200-0x42FF)](#system-control-0x4200-0x42ff)
      - [Video Control Registers (0x3680-0x369F)](#video-control-registers-0x3680-0x369f)
    - [System ROM Functions](#system-rom-functions)
      - [Graphics Functions](#graphics-functions)
      - [Audio Functions](#audio-functions)
      - [Input Functions](#input-functions)
      - [Math Functions](#math-functions)
      - [Memory/Utility Functions](#memoryutility-functions)
    - [CPU Architecture](#cpu-architecture)
      - [Registers](#registers)
      - [Status Flags](#status-flags)
    - [Instruction Set](#instruction-set)
      - [Instruction Formats](#instruction-formats)
    - [Specific Opcodes and categories](#specific-opcodes-and-categories)
      - [REG-Format Instructions](#reg-format-instructions)
      - [IMM-Format Instructions](#imm-format-instructions)
      - [JB-Format Instructions](#jb-format-instructions)
      - [SP-Format (and extra) Instructions](#sp-format-and-extra-instructions)
    - [Assembly Language](#assembly-language)
      - [Syntax Overview](#syntax-overview)
      - [Variables and Constants](#variables-and-constants)
      - [Addressing Modes](#addressing-modes)
      - [Psuedo-Instructions / Macros](#psuedo-instructions--macros)
      - [Enhanced NOP Instruction](#enhanced-nop-instruction)
  - [Development](#development)
    - [Development Tools](#development-tools)
    - [Testing](#testing)

## About

`IDN-16` is a 16-bit, simulated console featuring a custom CPU architecture, memory-mapped I/O, and a RISC-V inspired custom instruction set. This serves as a way for me to solidify what I learned about low-level computer architecture, emulation techniques, and graphics programming, while creating something that I find fun and creatively satisfying.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You will need the following software installed:

- `GCC` or `Clang` (C compiler)
- `Make`
- `SDL2` development libraries (for graphics, input handling, and sound)
- `Git` (to clone the repository)

For Ubuntu/Debian, these can be installed by running the following:
```bash
sudo apt update 
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev libtiff-dev libavif-dev libwebp-dev libjpeg-dev libpng-dev flex bison git
```

Or use the provided makefile target:
```bash
make install-deps
```

### Installing

**Clone the repository**
```bash
git clone https://github.com/yourusername/IDN-16.git
cd IDN-16
```

**Building the project**
```bash
make all
```

This will build the core emulator, assembler, and run the tests. To build specific components:

```bash
make core        # Build just the emulator
make asmblr      # Build just the assembler
make disasmblr   # Build just the disassembler
make test        # Run unit tests only
make dev         # Clean rebuild (development)
```

**Quick Start Example**
```bash
# Build everything
make all

# Create a simple program
echo "LDI r1, 42
      HLT" > hello.asm

# Assemble it
./asm hello.asm hello.bin

# Copy to ROM location
cp hello.bin src/core/roms/hello.bin

# Run the emulator
./idn16 --rom hello.bin
```

**Development Workflow**
```bash
# Edit your assembly file
vim program.asm

# Assemble and test
./asm program.asm test.bin && cp test.bin src/core/roms/system.bin && ./idn16
```

## Tools

### Assembler

The IDN-16 toolchain includes a custom assembler that compiles assembly language programs into binary machine code for the IDN-16 console.

**Building the assembler:**
```
make asmblr
```

**Usage:**
```bash
./asm input.asm output.bin
```

Where:
- `input.asm` is your assembly source file
- `output.bin` is the binary output file that can be loaded into the emulator

**Example:**
```bash
# Assemble startup.asm to system.bin
./asm startup.asm system.bin

# Assemble with variables and place in ROM directory
./asm program.asm program.bin
cp program.bin src/core/roms/system.bin
```

The assembler is a two-pass system that:
1. First pass: Collects all labels and their addresses
2. Second pass: Generates binary code with resolved label references

### Disassembler

A disassembler is provided to convert binary machine code back to readable assembly language.

**Building the disassembler:**
```
make disasmblr
```

**Usage:**
```bash
./dasm input.bin
```

Where:
- `input.bin` is the binary file containing IDN-16 machine code

**Example:**
```bash
# Disassemble a ROM file
./dasm src/core/roms/system.bin

# Disassemble any binary file
./dasm program.bin
```

The disassembler will generate output showing each assembly instruction:

```
Instruction
------------------------
LDI  r1, 0x05
LDI  r2, 0x01
CMP  r1, r2
JEQ  4
HLT
```

### Example Programs

Here's a simple factorial program to demonstrate the assembly language:

```assembly
; Calculate factorial of 5
start:
    LDI  r1, 5        ; Calculate factorial of 5
    LDI  r2, 1        ; Initialize result to 1
    
factorial_loop:
    CMP  r1, 0        ; Check if counter is zero
    JEQ  done         ; If zero, we're done
    
    MOV  r3, r2       ; Copy current result to r3
    MOV  r4, r1       ; Copy counter to r4
    
multiply_loop:
    ADDI r4, r4, -1   ; Decrement multiply counter
    JEQ  multiply_done ; If zero, multiplication is done
    ADD  r2, r2, r3   ; Add r3 to result
    JMP  multiply_loop
    
multiply_done:
    ADDI r1, r1, -1   ; Decrement factorial counter
    JMP  factorial_loop
    
done:
    STB   r2, [r7+0] ; Store result at address in r7
    HLT              ; Halt execution
```

## Documentation

### Memory Mapping
#### High-level Memory Layout
The `IDN-16` has a 64KB byte-addressable memory space sectioned into several dedicated regions:
```
0x0000-0x1FFF: User ROM (8KB user program storage, read-only)
0x2000-0x2FFF: RAM (4KB general purpose, read-write)
0x3000-0x3FFF: Video Memory (4KB for graphics system)
0x4000-0x40FF: Audio Registers (256 bytes)
0x4100-0x41FF: Input Registers (256 bytes)
0x4200-0x42FF: System Control (256 bytes for interrupts, timers)
0x4300-0xFFFF: System ROM (~47KB for built-in functions)
```

#### Video Memory Layout (0x3000-0x3FFF)
The video system uses a tile-based architecture with the following sub-regions:
```
0x3000-0x34FF: Tile Buffer (40x30 tiles = 1200 bytes)
0x3500-0x35FF: Sprite Table (64 sprites x 4 bytes = 256 bytes)
0x3600-0x367F: Palette RAM (64 colors x 2 bytes RGB565 = 128 bytes)
0x3680-0x369F: Video Control Registers (32 bytes)
0x36A0-0x3FFF: Tileset Data (256 tiles x 32 bytes each)
```

**Display Specifications:**
- Resolution: 320x240 pixels (40x30 tiles, 8x8 pixels per tile)
- Colors: 64-color palette using RGB565 format
- Sprites: Up to 64 hardware sprites with priority layers and flipping
- Video Modes: Text (simple programs), Tiles (backgrounds), Mixed (tiles + sprites for games)
- Features: Smooth scrolling backgrounds, no complex pixel manipulation needed
### IO-Interfacing
The IDN-16 uses memory-mapped I/O for interfacing with hardware systems:

#### Audio System (0x4000-0x40FF)
Audio processing and sound generation registers (256 bytes available for audio channels, waveform control, and effects).

#### Input System (0x4100-0x41FF)
**0x4100**: Controller 1 button state (read-only)
- Bit 0: F key (Action A)
- Bit 1: G key (Action B)
- Bit 2: Start
- Bit 3: Select
- Bit 4: W key (Up)
- Bit 5: S key (Down)
- Bit 6: A key (Left)
- Bit 7: D key (Right)

**0x4101**: Controller 2 button state (read-only)
- Bit 0: Action A
- Bit 1: Action B  
- Bit 2: Start
- Bit 3: Select
- Bit 4: Arrow Up
- Bit 5: Arrow Down
- Bit 6: Arrow Left
- Bit 7: Arrow Right

**0x4102 - 0x41FF**: Reserved for additional input devices

**Key Mappings:**
- **Player 1**: WASD for movement, F/G for action buttons
- **Player 2**: Arrow keys for movement
- **Debug**: F1 (Debug mode), F4 (Memory dump)

#### System Control (0x4200-0x42FF)
- **0x4200**: System status register
- **0x4201**: Interrupt control register
- **0x4202**: Timer counter (low byte)
- **0x4204**: Timer counter (high byte)
- **0x4206**: Frame counter (low byte)
- **0x4208**: Frame counter (high byte)

#### Video Control Registers (0x3680-0x369F)
- **0x3680**: Video mode register (Text=0, Tiles=1, Mixed=2)
- **0x3682**: Background scroll X position
- **0x3684**: Background scroll Y position
- **0x3686**: Text cursor X position
- **0x3688**: Text cursor Y position

### System ROM Functions
The IDN-16 includes a comprehensive System ROM with built-in functions accessible through the assembler. The assembler recognizes system function names, eliminating the need for manual address calculations. All functions follow a consistent calling convention where **r1 is the primary output register** whenever possible.

#### Graphics Functions
| Function            | Address | Inputs | Outputs | Description |
|-------------------- | ------- | ------ | ------- | ----------- |
| SYS_CLEAR_SCREEN    | 0x4300  | r1=fill_value | r1=success(1/0) | Clear the display with specified fill value |
| SYS_PUT_CHAR        | 0x4302  | r1=x, r2=y, r3=char | r1=success(1/0) | Display a character at position |
| SYS_PUT_STRING      | 0x4304  | r1=x, r2=y, r3=string_addr | r1=chars_displayed | Display a null-terminated string |
| SYS_SET_CURSOR      | 0x4306  | r1=x, r2=y | r1=success(1/0) | Set text cursor position |
| SYS_GET_CURSOR      | 0x4308  | - | r1=x, r2=y | Get current cursor position |
| SYS_SCROLL_SCREEN   | 0x430A  | r1=lines | r1=lines_scrolled | Scroll screen up by specified lines |
| SYS_SET_TILE        | 0x430C  | r1=x, r2=y, r3=tile_id | r1=success(1/0) | Place a tile in the tile buffer |
| SYS_GET_TILE        | 0x430E  | r1=x, r2=y | r1=tile_id | Get tile at position |
| SYS_CREATE_SPRITE   | 0x4310  | r1=id, r2=x, r3=y, r4=tile_id | r1=success(1/0) | Create and configure a sprite |
| SYS_MOVE_SPRITE     | 0x4312  | r1=id, r2=new_x, r3=new_y | r1=success(1/0) | Move sprite to new position |
| SYS_SET_SPRITE_TILE | 0x4314  | r1=sprite_id, r2=tile_id | r1=success(1/0) | Change sprite's tile |
| SYS_HIDE_SPRITE     | 0x4316  | r1=sprite_id, r2=hide(1/0) | r1=success(1/0) | Hide or show sprite |
| SYS_SET_PALETTE     | 0x4318  | r1=index, r2=color | r1=success(1/0) | Set palette color (RGB565) |
| SYS_GET_PALETTE     | 0x431A  | r1=index | r1=color | Get palette color |
| SYS_SET_VIDEO_MODE  | 0x431C  | r1=mode(0-2) | r1=success(1/0) | Change video mode (0=text, 1=tiles, 2=mixed) |
| SYS_SET_SCROLL      | 0x431E  | r1=scroll_x, r2=scroll_y | r1=success(1/0) | Set background scroll position |

#### Audio Functions  
| Function          | Address | Inputs | Outputs | Description |
| ----------------- | ------- | ------ | ------- | ----------- |
| SYS_PLAY_TONE     | 0x4400  | r1=freq, r2=duration, r3=channel | r1=success(1/0) | Play tone on audio channel (0-3) |
| SYS_STOP_CHANNEL  | 0x4402  | r1=channel | r1=success(1/0) | Stop audio on specific channel |
| SYS_SET_VOLUME    | 0x4404  | r1=channel, r2=volume(0-15) | r1=success(1/0) | Set channel volume |
| SYS_SET_WAVEFORM  | 0x4406  | r1=channel, r2=waveform(0-3) | r1=success(1/0) | Set waveform (0=square, 1=sine, 2=triangle, 3=noise) |

#### Input Functions
| Function | Address | Inputs | Outputs | Description |
|----------|---------|--------|---------|-------------|
| SYS_GET_KEYS | 0x4500 | r1=controller(0/1) | r1=button_state | Read current controller state |
| SYS_WAIT_KEY | 0x4502 | - | r1=key_pressed | Wait for any key press |
| SYS_KEY_PRESSED | 0x4504 | r1=controller, r2=key_bit(0-7) | r1=pressed(1/0) | Check if specific key is pressed |

#### Math Functions
| Function | Address | Inputs | Outputs | Description |
|----------|---------|--------|---------|-------------|
| SYS_MULTIPLY | 0x4600 | r1=multiplicand, r2=multiplier | r1=result_low, r2=result_high | 16-bit multiplication |
| SYS_DIVIDE | 0x4602 | r1=dividend, r2=divisor | r1=quotient, r2=remainder | 16-bit division |
| SYS_RANDOM | 0x4604 | - | r1=random_number | Generate random number |
| SYS_ABS | 0x4606 | r1=signed_value | r1=absolute_value | Get absolute value |

#### Memory/Utility Functions
| Function | Address | Inputs | Outputs | Description |
|----------|---------|--------|---------|-------------|
| SYS_MEMCPY | 0x4700 | r1=dest, r2=src, r3=length | r1=bytes_copied | Copy memory block |
| SYS_MEMSET | 0x4702 | r1=dest, r2=value, r3=length | r1=bytes_set | Fill memory block with value |
| SYS_STRLEN | 0x4704 | r1=string_addr | r1=length | Get string length |
| SYS_STRCMP | 0x4706 | r1=string1, r2=string2 | r1=result(-1/0/1) | Compare strings |
| SYS_STRCPY | 0x4708 | r1=dest, r2=src | r1=dest_addr | Copy string |

**Usage Examples:**

**Basic Text Display:**
```assembly
; Clear screen with space character
LDI r1, 32              ; Space character
JSR SYS_CLEAR_SCREEN    ; r1 = success

; Set cursor position  
LDI r1, 10              ; x = 10
LDI r2, 5               ; y = 5
JSR SYS_SET_CURSOR      ; r1 = success

; Display a string
LDI r1, 0               ; x = 0 
LDI r2, 0               ; y = 0
LOAD16 r3, hello_msg    ; String address
JSR SYS_PUT_STRING      ; r1 = characters displayed

hello_msg:
    .string "Hello IDN-16!"
```

**Graphics and Sprites:**
```assembly
; Set video mode to mixed (tiles + sprites)
LDI r1, 2               ; Mixed mode
JSR SYS_SET_VIDEO_MODE  ; r1 = success

; Set a tile
LDI r1, 5               ; x = 5
LDI r2, 3               ; y = 3  
LDI r3, 42              ; tile_id = 42
JSR SYS_SET_TILE        ; r1 = success

; Create a sprite
LDI r1, 0               ; sprite_id = 0
LDI r2, 100             ; x = 100
LDI r3, 50              ; y = 50
LDI r4, 15              ; tile_id = 15
JSR SYS_CREATE_SPRITE   ; r1 = success

; Move the sprite
LDI r1, 0               ; sprite_id = 0
LDI r2, 120             ; new_x = 120
LDI r3, 60              ; new_y = 60
JSR SYS_MOVE_SPRITE     ; r1 = success
```

**Audio Example:**
```assembly
; Play a tone
LDI r1, 440             ; 440 Hz (A note)
LDI r2, 1000            ; Duration (implementation defined)
LDI r3, 0               ; Channel 0
JSR SYS_PLAY_TONE       ; r1 = success

; Set volume
LDI r1, 0               ; Channel 0
LDI r2, 8               ; Volume level (0-15)
JSR SYS_SET_VOLUME      ; r1 = success
```

**Input Handling:**
```assembly
input_loop:
    LDI r1, 0           ; Controller 0
    JSR SYS_GET_KEYS    ; r1 = button state
    CMP r1, r0          ; Check if any buttons pressed
    JEQ input_loop      ; Loop if no input
    
    ; Check specific button (bit 0 = Action A)
    LDI r1, 0           ; Controller 0  
    LDI r2, 0           ; Bit 0 (Action A)
    JSR SYS_KEY_PRESSED ; r1 = 1 if pressed, 0 if not
    CMP r1, r0
    JEQ input_loop      ; Loop if Action A not pressed
    
    ; Action A was pressed, do something
    ; ...
```

### CPU Architecture
#### Registers
The `IDN-16`'s CPU has 8 16-bit registers:
| Register      | Purpose         |
| ------------- | --------------- |
| r0 / zero     | Always Zero     |
| r1            | General         |
| r2            | General         |
| r3            | General         |
| r4            | General         |
| r5            | General         |
| r6 / sp       | Stack Pointer   |
| r7 / ra       | Return Address  |
#### Status Flags
Statuses are stored with a special flag register:
- Zero (Z): Set when result is zero
- Negative (N): Set when result is negative
- Carry (C): Set when operation produces carry
- Overflow (V): Set when signed overflow occurs

### Instruction Set
#### Instruction Formats
All instructions are 16 bits wide:
| Formats                                 | Bit Allocation                                                  |
| --------------------------------------- | --------------------------------------------------------------- |
| REG-format (Register Operations)        | [5 bits opcode][3 bits rd][3 bits rs1][3 bits rs2][2 bits func] |
| IMM-Format (Immediate/Memory operations)| [5 bits opcode][3 bits rd][3 bits rs1][5 bits immediate]        |
| JB-Format (Jump/Branch operations)      | [5 bits opcode][11 bits jump offset]                            |
| SP-Format (Special operations)          | [5 bits opcode][3 bits rd][8 bits varies by instruction]        |

### Specific Opcodes and categories
#### REG-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                               |
| ------- | ------------------------------------- | ------------------------------------|
| 00000   | ADD rd, rs1, rs2                      | Add registers                       |
| 00001   | SUB rd, rs1, rs2                      | Subtract registers                  |
| 00010   | AND rd, rs1, rs2                      | Bitwise AND                         |
| 00011   | OR rd, rs1, rs2                       | Bitwise OR                          |
| 00100   | XOR rd, rs1, rs2                      | Bitwise XOR                         |
| 00101   | SHL rd, rs1, rs2                      | Shift left                          |
| 00110   | SHR rd, rs1, rs2 (func=00)            | Shift right logical                 |
| 00110   | SRA rd, rs1, rs2 (func=01)            | Shift right arithmetic              |
| 00111   | MOV rd, rs1 (func=00)                 | Copy register                       |
| 00111   | CMP rd, rs1 (func=01)                 | Compare regs and set flags          |
| 00111   | NOT rd, rs1 (func=10)                 | Bitwise NOT                         |

#### IMM-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                               |
| ------- | ------------------------------------- | ------------------------------------|
| 01000   | LDI rd, imm (8 bits)                  | Load immediate (MSB Extends)        |
| 01001   | LDW rd, [rs1+imm]                     | Load word from memory               |
| 01010   | STW rd, [rs1+imm]                     | Store word to memory                |
| 01011   | ADDI rd, rs1, imm                     | Add immediate (MSB Extends)         |
| 01100   | LUI rd, imm (8 bits)                  | Load immediate to upper 8 bits      |
| 01101   | ANDI rd, rs1, imm                     | AND immediate                       |
| 01110   | ORI rd, rs1, imm                      | OR immediate                        |
| 01111   | XORI rd, rs1, imm                     | XOR immediate                       |

#### JB-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                               |
| ------- | ------------------------------------- | ------------------------------------|
| 10000   | JMP offset                            | Unconditional jump                  |
| 10001   | JEQ offset                            | Jump if equal                       |
| 10010   | JNE offset                            | Jump if not equal                   |
| 10011   | JGT offset                            | Jump if greater than                |
| 10100   | JLT offset                            | Jump if less than                   |
| 10101   | JSR offset                            | Jump to subroutine + store pc in ra |
| 10110   | RET                                   | Return from subroutine              |

#### SP-Format (and extra) Instructions

| Opcode  | Assembly Syntax                       | Usage                               |
| ------- | ------------------------------------- | ------------------------------------|
| 11000   | HLT                                   | Halt processor                      |
| 11001   | NOP count                             | Insert 'count' NOP instructions     |
| 11010   | INC rd                                | Increment register                  |
| 11011   | DEC rd                                | Decrement register                  |
| 11100   | LDB rd, [rs1+imm]                     | Load byte from memory               |
| 11101   | STB rd, [rs1+imm]                     | Store byte to memory                |
### Assembly Language
#### Syntax Overview
```
; This is a comment
label:          ; Define a label
  LDI  sp, 10   ; Load immediate value 10 into r6
  JSR  routine  ; Jump to subroutine "routine"
  HLT           ; Halt execution

routine:
  MOV  r1, sp   ; Copy r6 to r1
  RET           ; Return from subroutine
```

#### Variables and Constants
The assembler supports variable and constant definitions that can be used throughout your program:

```assembly
; Define constants
SCREEN_WIDTH = 320
SCREEN_HEIGHT = 240
MAX_SPRITES = 64

; Define memory addresses
VIDEO_BASE = 0x3000
INPUT_REG = 0x4100

; Use in instructions
LDI r1, SCREEN_WIDTH    ; Load 320 into r1
LOAD16 r2, VIDEO_BASE   ; Load 0x3000 into r2
LDW r3, [INPUT_REG]     ; Load from address 0x4100
```

**Variable Assignment Syntax:**
- `IDENTIFIER = value` - Define a constant or memory address
- Values can be decimal, hexadecimal (`0x1234`), binary (`0b1010`), or character literals (`'A'`)
- Variables can be used anywhere an immediate value is expected
- Case-insensitive (both `VIDEO_BASE` and `video_base` work)
#### Addressing Modes
- **Immediate:** LDI r1, 5
- **Character:** LDI r1, 'a'
- **Register:** MOV r1, r2
- **Register Indirect with Offset:** LD r2, [r1+2]
#### Psuedo-Instructions / Macros
The assembler has support for pseudo-instructions / macros:
- **CALL** addr -> 
  - ```
    JSR addr
    ```
- **INC** reg -> 
  - ```
    ADDI reg, reg, 1
    ```
- **DEC** reg ->
  - ```
    ADDI reg, reg, -1
    ```
- **LOAD16** reg, imm16 ->
  - ```
    LDI reg, imm16[7:0]
    LUI reg, imm16[15:8]
    ```

#### Enhanced NOP Instruction
The NOP instruction supports multiple forms for efficient padding and alignment:

```assembly
; Basic NOP
NOP                     ; Single no-operation

; Multiple NOPs with immediate values
NOP 5                   ; Insert 5 NOP instructions
NOP 0x10                ; Insert 16 NOP instructions  
NOP 255                 ; Insert 255 NOP instructions

; Multiple NOPs using constants
PADDING_SIZE = 128
NOP PADDING_SIZE        ; Insert 128 NOP instructions

; Practical usage for memory alignment
audio_functions:
    NOP 112             ; Pad to specific address
    ; functions start here...
```

**Supported NOP Formats:**
- `NOP` - Single NOP instruction
- `NOP immediate` - Multiple NOPs
- `NOP identifier` - Multiple NOPs using defined constants

This is particularly useful for:
- Memory alignment and padding
- Function table organization  
- Creating gaps for future expansion
- System ROM address management
## Development

### Development Tools

The IDN-16 includes comprehensive development and debugging tools:

**Assembler Features:**
- Complete assembler with proper syntax highlighting support
- System ROM function name recognition (no manual address calculations)
- Label resolution and macro support
- Error reporting with line numbers

**Runtime Debug Features:**
- **F1**: Toggle debug mode for real-time system monitoring
- **F4**: Memory dump to inspect memory regions
- ROM loading support for easy program deployment
- Real-time display of CPU state and memory contents

**Graphics Development:**
- Tile-based rendering system (40×30 tiles of 8×8 pixels)
- Visual tile editor support through memory-mapped interface
- Sprite management with hardware acceleration
- Palette editor through direct memory access

### Testing

The project includes comprehensive test suites for both the CPU and memory subsystems. These tests verify the correct behavior of each instruction and memory operation.

**Running the tests:**
```bash
make test
```

**Individual test components:**
```bash
make run_test_memory    # Test memory subsystem only
make run_test_cpu       # Test CPU subsystem only
```

The test suite uses Unity as the testing framework and validates:
- All CPU instructions (ADD, SUB, LDI, etc.)
- Memory read/write operations
- Memory-mapped I/O behavior
- Edge cases like register r0 operations, overflow conditions

---
This documentation will be expanded as development continues.
