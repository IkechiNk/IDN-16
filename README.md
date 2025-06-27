# `IDN-16` Console
## Table of Contents
<!-- NOTE: WHEN GETTING "{lib}.dll" is not a .so shared library, this problem is becuase cmake is returning the windows path to the libary.-->
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
      - [Video Memory Layout (0xD000-0xEFFF)](#video-memory-layout-0xd000-0xefff)
    - [IO-Interfacing](#io-interfacing)
      - [Audio System (0xF000-0xF0FF)](#audio-system-0xf000-0xf0ff)
      - [Input System (0xF100-0xF1FF)](#input-system-0xf100-0xf1ff)
      - [System Control (0xF200-0xF2FF)](#system-control-0xf200-0xf2ff)
      - [Video Control Registers (0xD4B0-0xD4CF)](#video-control-registers-0xd4b0-0xd4cf)
    - [System Architecture](#system-architecture)
    - [System Call Functions](#system-call-functions)
      - [Text/Console Functions](#textconsole-functions)
      - [Sprite Animation Functions](#sprite-animation-functions)
      - [Timer Functions](#timer-functions)
      - [Audio Functions](#audio-functions)
      - [Input Functions](#input-functions)
      - [Math Functions](#math-functions)
      - [Memory/Utility Functions](#memoryutility-functions)
    - [Sprite System (Direct Memory Access)](#sprite-system-direct-memory-access)
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

**Hello World (Grid-based)**
```assembly
CHAR_H = 72
CHAR_E = 69
CHAR_L = 76
CHAR_O = 79

start:
    ; Clear screen
    LOAD16 r1, 0xF300
    JSR r1
    
    ; Display "HELLO" using cursor
    LDI r1, CHAR_H
    LOAD16 r2, 0xF301
    JSR r2
    
    LDI r1, CHAR_E
    JSR r2
    
    LDI r1, CHAR_L
    JSR r2
    JSR r2                  ; display 'L' twice
    
    LDI r1, CHAR_O
    JSR r2
    
    HLT
```

**Centered Text (Grid-based)**
```assembly
; Draw "IDN-16" centered on screen using cursor positioning
start:
    LOAD16 r1, 0xF300       ; clear screen
    JSR r1
    
    ; Position cursor at center (40x30 grid, so center is around 17,15)
    LDI r1, 17              ; center x (character position)
    LDI r2, 15              ; center y (character position)
    LOAD16 r3, 0xF303       ; set cursor
    JSR r3
    
    ; Display "IDN-16" using PUT_CHAR
    LDI r1, 'I'
    LOAD16 r2, 0xF301       ; PUT_CHAR
    JSR r2
    
    LDI r1, 'D'
    JSR r2
    
    LDI r1, 'N'
    JSR r2
    
    LDI r1, '-'
    JSR r2
    
    LDI r1, '1'
    JSR r2
    
    LDI r1, '6'
    JSR r2
    
    HLT
```

**Input Handling**
```assembly
input_loop:
    LDI r1, 0               ; controller 0
    LOAD16 r2, 0xF309       ; GET_INPUT
    JSR r2                  ; r1 = button state
    
    CMP r1, r0              ; check if any buttons pressed
    JEQ input_loop          ; loop if no input
    
    ; Button pressed - display something
    LDI r1, 42              ; asterisk character
    LOAD16 r2, 0xF301       ; PUT_CHAR
    JSR r2
    
    JMP input_loop
```

**Factorial Calculator**
```assembly
; Calculate factorial of 5
factorial_demo:
    LDI r1, 5               ; number to calculate
    LDI r2, 1               ; result accumulator
    
factorial_loop:
    CMP r1, r0              ; check if done
    JEQ factorial_done
    
    ; Multiply r2 by r1 using system call
    LOAD16 r3, 0xF30B       ; MULTIPLY syscall
    JSR r3                  ; r1 = r2 * r1
    MOV r2, r1              ; store result
    
    DEC r1                  ; decrement counter
    JMP factorial_loop
    
factorial_done:
    ; Display result as decimal
    MOV r1, r2
    LOAD16 r2, 0xF310       ; PRINT_DEC
    JSR r2
    HLT
```

**Simple Red Square Sprite Demo:**
```assembly
; IDN-16 Red Square Sprite Demo
; Creates a red 8x8 square sprite at center of display

; Memory addresses
PALETTE_RAM_START = 0xD4D0        ; Palette colors (RGB565)
SPRITE_TABLE_START = 0xD4F0       ; Sprite positions and tile IDs
TILESET_DATA_START = 0xD5B0       ; Tile pixel data

start:
    ; Set up red color in palette slot 1
    LOAD16 r1, PALETTE_RAM_START   ; Palette base address
    ADDI r1, r1, 2                 ; Palette slot 1 (2 bytes per color)
    LOAD16 r2, 0xF800              ; Red color (RGB565: 11111 000000 00000)
    STW r2, [r1+0]                 ; Write red to palette slot 1
    
    ; Create red square tile (tile 1) - fill all 64 pixels
    LOAD16 r1, TILESET_DATA_START  ; Tile 0 base
    ADDI r1, r1, 64                ; Move to tile 1 (64 bytes per tile)
    LDI r2, 1                      ; Palette index 1 (red)
    LDI r3, 0                      ; Pixel counter
    
fill_tile:
    STB r2, [r1+0]                 ; Write red pixel
    ADDI r1, r1, 1                 ; Next pixel address
    ADDI r3, r3, 1                 ; Increment counter
    LDI r4, 64                     ; Total pixels
    CMP r3, r4                     ; Check if done
    JNE fill_tile                  ; Continue if not done
    
    ; Position sprite at center (20x15 tiles = center of 40x30 grid)
    LOAD16 r1, SPRITE_TABLE_START  ; Sprite table base
    LDI r2, 20                     ; X position (center)
    STB r2, [r1+0]                 ; Write X coordinate
    LDI r2, 15                     ; Y position (center)
    STB r2, [r1+1]                 ; Write Y coordinate
    LDI r2, 1                      ; Tile ID 1 (our red square)
    STB r2, [r1+2]                 ; Write tile ID
    
    HLT
```

## Documentation

### Memory Mapping
#### High-level Memory Layout
The `IDN-16` has a 64KB byte-addressable memory space sectioned into several dedicated regions:
```
0x0000-0x7FFF: User ROM (32KB user program storage, read-only)
0x8000-0xCFFF: RAM (20KB general purpose, read-write)
0xD000-0xEFFF: Video Memory (8KB for graphics system)
0xF000-0xF0FF: Audio Registers (256 bytes)
0xF100-0xF1FF: Input Registers (256 bytes)
0xF200-0xF2FF: System Control (256 bytes for interrupts, timers)
0xF300-0xFFFF: System Calls (3328 bytes for system call range)
```

#### Video Memory Layout (0xD000-0xEFFF)
The video system uses a tile-based architecture with the following sub-regions:
```
0xD000-0xD4AF: Character Buffer (40x30 characters = 1200 bytes)
0xD4B0-0xD4CF: Video Control Registers (32 bytes)
0xD4D0-0xD4EF: Palette RAM (16 colors x 2 bytes RGB565 = 32 bytes)
0xD4F0-0xD5AF: Sprite Table (64 sprites x 3 bytes = 192 bytes)
0xD5B0-0xEFFF: Tileset Data (105 tiles x 64 bytes each, 8x8 pixels per tile)
```

**Display Specifications:**
- Resolution: 320x240 pixels (40x30 tiles, 8x8 pixels per tile)
- Colors: 16-color palette using RGB565 format
- Sprites: Up to 64 hardware sprites with priority layers and flipping
- Architecture: Dual-mode system (text via syscalls, sprites via memory)
- Features: Smooth scrolling backgrounds, no complex pixel manipulation needed
### IO-Interfacing
The IDN-16 uses memory-mapped I/O for interfacing with hardware systems:

#### Audio System (0xF000-0xF0FF)
Audio processing and sound generation registers (256 bytes available for audio channels, waveform control, and effects).

#### Input System (0xF100-0xF1FF)
**0xF100**: Controller 1 button state (read-only)
- Bit 0: F key (Action A)
- Bit 1: G key (Action B)
- Bit 2: Start
- Bit 3: Select
- Bit 4: W key (Up)
- Bit 5: S key (Down)
- Bit 6: A key (Left)
- Bit 7: D key (Right)

**0xF101**: Controller 2 button state (read-only)
- Bit 0: Action A
- Bit 1: Action B  
- Bit 2: Start
- Bit 3: Select
- Bit 4: Arrow Up
- Bit 5: Arrow Down
- Bit 6: Arrow Left
- Bit 7: Arrow Right

**0xF102 - 0xF1FF**: Reserved for additional input devices

**Key Mappings:**
- **Player 1**: WASD for movement, F/G for action buttons
- **Player 2**: Arrow keys for movement
- **Debug**: F1 (Debug mode), F4 (Memory dump)

#### System Control (0xF200-0xF2FF)
- **0xF200**: System status register
- **0xF201**: Interrupt control register
- **0xF202**: Timer counter (low byte)
- **0xF203**: Timer counter (high byte)
- **0xF206**: Frame counter (low byte)
- **0xF208**: Frame counter (high byte)

#### Video Control Registers (0xD4B0-0xD4CF)
- **0xD4B0**: Text cursor X position
- **0xD4B2**: Text cursor Y position

### System Architecture

The IDN-16 uses a **dual-mode graphics system**:

1. **Text System**: Controlled by system calls (0xF300+) for console-style character output
2. **Sprite System**: Controlled by direct memory writes to video memory (0xD000+)

**Text operations** use system calls and work like a traditional console.
**Sprite operations** require direct memory manipulation for maximum performance.

### System Call Functions
System calls handle text output, input, audio, math, sprite animation, and utility functions. These are accessed via specific memory addresses in the 0xF300-0xFFFF range. The IDN-16 now includes **35 total system calls** with enhanced sprite animation capabilities for game development.

#### Text/Console Functions
| Function            | Address | Inputs | Outputs | Description |
|-------------------- | ------- | ------ | ------- | ----------- |
| SYSCALL_CLEAR_SCREEN| 0xF300  | - | r1=success(1/0) | Clear entire 40x30 text screen |
| SYSCALL_PUT_CHAR    | 0xF301  | r1=char | r1=success(1/0) | Put 8x8 character at cursor position |
| SYSCALL_PUT_STRING  | 0xF302  | r1=string_addr, r2=max_length | r1=success(1/0) | Put string using 8x8 font |
| SYSCALL_SET_CURSOR  | 0xF303  | r1=x, r2=y | - | Set text cursor position (x, y) |
| SYSCALL_GET_CURSOR  | 0xF304  | - | r1=x, r2=y | Get current cursor position |
| SYSCALL_PUT_CHAR_AT | 0xF305  | r1=x, r2=y, r3=char | - | Put character at specific position |
| SYSCALL_SCROLL_UP   | 0xF306  | - | - | Scroll text screen up one line |
| SYSCALL_FILL_AREA   | 0xF307  | r1=x, r2=y, r3=width, r4=height, r5=char | - | Fill rectangular area with character |
| SYSCALL_SET_TEXT_COLOR | 0xF308 | r1=fg_color, r2=bg_color | - | Set foreground/background color |
| SYSCALL_GET_INPUT   | 0xF309  | r1=controller | r1=button_state | Get keyboard input |
| SYSCALL_PLAY_TONE   | 0xF30A  | r1=freq, r2=duration | r1=success(1/0) | Play audio tone |
| SYSCALL_MULTIPLY    | 0xF30B  | r1=a, r2=b | r1=result_low, r2=result_high | 16-bit multiply operation |
| SYSCALL_DIVIDE      | 0xF30C  | r1=dividend, r2=divisor | r1=quotient, r2=remainder | 16-bit divide operation |
| SYSCALL_RANDOM      | 0xF30D  | - | r1=random_number | Generate random number |
| SYSCALL_MEMCPY      | 0xF30E  | r1=dest, r2=src, r3=length | r1=bytes_copied | Memory copy operation |
| SYSCALL_PRINT_HEX   | 0xF30F  | r1=number | - | Print number in hexadecimal |
| SYSCALL_PRINT_DEC   | 0xF310  | r1=number | - | Print number in decimal |
| SYSCALL_SET_SPRITE  | 0xF311  | r1=sprite_id, r2=x, r3=y, r4=tile_id | r1=success(1/0) | Set sprite position and tile |
| SYSCALL_SET_PALETTE | 0xF312  | r1=palette_index, r2=color | r1=success(1/0) | Set palette color (RGB565) |
| SYSCALL_MOVE_SPRITE | 0xF313  | r1=sprite_id, r2=new_x, r3=new_y | r1=success(1/0) | Move sprite to new position |
| SYSCALL_SET_SPRITE_PIXEL | 0xF314 | r1=tile_id, r2=pixel_x, r3=pixel_y, r4=palette_index | r1=success(1/0) | Set individual sprite pixel color |
| SYSCALL_GET_FRAME_COUNT | 0xF315 | - | r1=frame_count | Get current frame count |

#### Sprite Animation Functions
| Function            | Address | Inputs | Outputs | Description |
|-------------------- | ------- | ------ | ------- | ----------- |
| SYSCALL_HIDE_SPRITE | 0xF316  | r1=sprite_id | r1=success(1/0) | Hide sprite by moving off screen |
| SYSCALL_GET_SPRITE_POS | 0xF317 | r1=sprite_id | r1=x, r2=y | Get sprite x,y position |
| SYSCALL_CLEAR_SPRITE_RANGE | 0xF318 | r1=start_id, r2=end_id | r1=sprites_cleared | Clear sprite range |
| SYSCALL_CHECK_COLLISION | 0xF319 | r1=sprite1_id, r2=sprite2_id | r1=collision(1/0) | Check 8x8 sprite collision |
| SYSCALL_SHIFT_SPRITES | 0xF31A | r1=start_id, r2=count, r3=dx, r4=dy | r1=sprites_moved | Move multiple sprites with offset |
| SYSCALL_COPY_SPRITE | 0xF31B | r1=src_id, r2=dest_id | r1=success(1/0) | Copy sprite properties |
| SYSCALL_SET_RETURN_ADDR | 0xF31C | - | r1=success(1/0) | Set return address register (r7) to next instruction |
| SYSCALL_MOVE_SPRITE_RIGHT | 0xF31D | r1=sprite_id, r2=tiles | r1=success(1/0) | Move sprite right by N tiles |
| SYSCALL_MOVE_SPRITE_LEFT | 0xF31E | r1=sprite_id, r2=tiles | r1=success(1/0) | Move sprite left by N tiles |
| SYSCALL_MOVE_SPRITE_UP | 0xF31F | r1=sprite_id, r2=tiles | r1=success(1/0) | Move sprite up by N tiles |
| SYSCALL_MOVE_SPRITE_DOWN | 0xF320 | r1=sprite_id, r2=tiles | r1=success(1/0) | Move sprite down by N tiles |

#### Timer Functions
| Function            | Address | Inputs | Outputs | Description |
|-------------------- | ------- | ------ | ------- | ----------- |
| SYSCALL_TIMER_START | 0xF321 | r1=duration | r1=success(1/0) | Start timer with duration in milliseconds |
| SYSCALL_TIMER_QUERY | 0xF322 | r1=stop_flag | r1=0 if complete, remaining_ms if not; r2=timer_state (1=stopped, 0=running) | Query timer status, optionally stop timer if r1=1 |
| SYSCALL_SLEEP | 0xF323 | r1=duration | r1=success(1/0) | Set CPU sleep timer in milliseconds |

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
; Clear screen 
LOAD16 r1, 0xF300       ; SYSCALL_CLEAR_SCREEN
JSR r1                  ; r1 = success

; Set cursor position  
LDI r1, 10              ; x = 10
LDI r2, 5               ; y = 5
LOAD16 r3, 0xF303       ; SYSCALL_SET_CURSOR
JSR r3                  ; Set cursor

; Display a character
LDI r1, 'H'             ; Character 'H'
LOAD16 r2, 0xF301       ; SYSCALL_PUT_CHAR
JSR r2                  ; r1 = success
```

**Stack Management Example:**
```assembly
; Subroutine that preserves registers using PUSH/POP
subroutine_example:
    ; Initialize stack pointer (typically done at program start)
    LOAD16 sp, 0xCFFF       ; Set stack to top of RAM
    
    ; Save registers before computation
    PUSH r1
    PUSH r2
    PUSH r3
    
    ; Perform some computation
    LDI r1, 10
    LDI r2, 20
    ADD r3, r1, r2          ; r3 = r1 + r2
    
    ; Call another subroutine (demonstrates nested calls)
    LOAD16 r1, helper_function
    JSR r1
    
    ; Restore registers in reverse order
    POP r3
    POP r2
    POP r1
    
    RET

helper_function:
    ; This function can use PUSH/POP too
    PUSH r4
    PUSH r5
    
    ; Do some work...
    LDI r4, 42
    LDI r5, 24
    ADD r4, r4, r5
    
    ; Restore and return
    POP r5
    POP r4
    RET
```

**Snake Game Example (using new sprite animation calls):**
```assembly
; Snake game movement logic using new animation system calls
snake_update:
    ; Check collision between snake head (sprite 0) and food (sprite 1)
    LDI r1, 0               ; Snake head sprite ID
    LDI r2, 1               ; Food sprite ID  
    LOAD16 r3, 0xF319       ; SYSCALL_CHECK_COLLISION
    JSR r3                  ; r1 = collision result
    
    CMP r1, r0              ; Check if collision occurred
    JEQ no_food             ; Jump if no collision
    
    ; Food eaten - grow snake and create new food
    LDI r1, 2               ; Start from snake body sprite 2
    LDI r2, 10              ; Number of body segments  
    LDI r3, -8              ; Move left by 8 pixels
    LDI r4, 0               ; No vertical movement
    LOAD16 r5, 0xF31A       ; SYSCALL_SHIFT_SPRITES
    JSR r5                  ; Move snake body
    
    ; Hide old tail and create new food
    LDI r1, 12              ; Last tail sprite
    LOAD16 r2, 0xF316       ; SYSCALL_HIDE_SPRITE
    JSR r2                  ; Hide tail
    
no_food:
    ; Continue game logic...

**Directional Sprite Movement Example:**
```assembly
; Simple player movement using new directional syscalls
player_movement:
    ; Get input from controller 0
    LDI r1, 0               ; Controller 0
    LOAD16 r2, 0xF309       ; SYSCALL_GET_INPUT
    JSR r2                  ; r1 = button state
    
    ; Check for movement input
    LDI r3, 0x10            ; Up button bit (bit 4)
    AND r4, r1, r3
    CMP r4, r0
    JEQ check_down
    
    ; Move player sprite up by 1 tile
    LDI r1, 0               ; Player sprite ID
    LDI r2, 1               ; Move 1 tile
    LOAD16 r3, 0xF31F       ; SYSCALL_MOVE_SPRITE_UP
    JSR r3
    JMP movement_done
    
check_down:
    LDI r3, 0x20            ; Down button bit (bit 5)
    AND r4, r1, r3
    CMP r4, r0
    JEQ check_left
    
    ; Move player sprite down by 1 tile
    LDI r1, 0               ; Player sprite ID
    LDI r2, 1               ; Move 1 tile
    LOAD16 r3, 0xF320       ; SYSCALL_MOVE_SPRITE_DOWN
    JSR r3
    JMP movement_done
    
check_left:
    LDI r3, 0x40            ; Left button bit (bit 6)
    AND r4, r1, r3
    CMP r4, r0
    JEQ check_right
    
    ; Move player sprite left by 1 tile
    LDI r1, 0               ; Player sprite ID
    LDI r2, 1               ; Move 1 tile
    LOAD16 r3, 0xF31E       ; SYSCALL_MOVE_SPRITE_LEFT
    JSR r3
    JMP movement_done
    
check_right:
    LDI r3, 0x80            ; Right button bit (bit 7)
    AND r4, r1, r3
    CMP r4, r0
    JEQ movement_done
    
    ; Move player sprite right by 1 tile
    LDI r1, 0               ; Player sprite ID
    LDI r2, 1               ; Move 1 tile
    LOAD16 r3, 0xF31D       ; SYSCALL_MOVE_SPRITE_RIGHT
    JSR r3
    
movement_done:
    ; Continue with other game logic...
```

### Sprite System (Direct Memory Access)

Sprites are controlled by writing directly to video memory regions:

**Memory Layout:**
- **Palette RAM**: 0xD4D0-0xD4EF (16 colors × 2 bytes, RGB565 format)
- **Sprite Table**: 0xD4F0-0xD5AF (64 sprites × 3 bytes: [X (up to 40), Y (up to 30), Tile_ID])
- **Tileset Data**: 0xD5B0-0xEFFF (105 tiles × 64 bytes each: 8×8 pixel data, 1 byte palette index per pixel)

**Sprite System Rules:**
- Sprites are disabled when Tile_ID = 0
- Palette index 0 = transparent (not rendered)
- Palette indices 1-15 = visible colors
- Palette indices 16+ = use previous valid palette color (1-15) from the current tile

**Creating Sprites with Direct Memory Writes:**
```assembly
; Memory addresses
PALETTE_RAM_START = 0xD4D0
SPRITE_TABLE_START = 0xD4F0
TILESET_DATA_START = 0xD5B0

; Set up red color in palette slot 1
LOAD16 r1, PALETTE_RAM_START
ADDI r1, r1, 2                 ; Palette slot 1 (2 bytes per color)
LOAD16 r2, 0xF800              ; Red color (RGB565)
STW r2, [r1+0]                 ; Write to palette

; Create red tile data (tile 0)
LOAD16 r1, TILESET_DATA_START  ; Tile 0 base address
LDI r2, 1                      ; Palette index 1 (red)
STB r2, [r1+0]                 ; Pixel (0,0)
STB r2, [r1+1]                 ; Pixel (1,0)
STB r2, [r1+8]                 ; Pixel (0,1) - next row
STB r2, [r1+9]                 ; Pixel (1,1)
; ... continue for 8x8 pixels

; Set up sprite 0 in sprite table
LOAD16 r1, SPRITE_TABLE_START  ; Sprite table base
LDI r2, 100                    ; X position
STB r2, [r1+0]                 ; Write X coordinate
LDI r2, 80                     ; Y position
STB r2, [r1+1]                 ; Write Y coordinate
LDI r2, 1                      ; Tile ID 1 (Tile_ID 0 = disabled)
STB r2, [r1+2]                 ; Write tile ID
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
| Register      | Purpose         | Notes                    |
| ------------- | --------------- | ------------------------ |
| r0 / zero     | Always Zero     | **Hardwired to 0 - writes ignored** |
| r1            | General         |                          |
| r2            | General         |                          |
| r3            | General         |                          |
| r4            | General         |                          |
| r5            | General         |                          |
| r6 / sp       | Stack Pointer   |                          |
| r7 / ra       | Return Address  |                          |

**Important:** Register r0 is hardwired to always contain zero. Any attempt to write to r0 will be ignored and the register will remain 0. This is enforced in hardware and cannot be overridden.
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

| Opcode  | Assembly Syntax                       | Usage                               | Immediate Size & Range |
| ------- | ------------------------------------- | ----------------------------------- | ---------------------- |
| 01000   | LDI rd, imm                           | Load immediate (sign extends)       | 8 bits: -128 to +255   |
| 01001   | LDW rd, [rs1+imm]                     | Load word from memory               | 5 bits: -16 to +31     |
| 01010   | STW rd, [rs1+imm]                     | Store word to memory                | 5 bits: -16 to +31     |
| 01011   | ADDI rd, rs1, imm                     | Add immediate (sign extends)        | 5 bits: -16 to +31     |
| 01100   | LUI rd, imm                           | Load immediate to upper 8 bits      | 8 bits: 0 to 255       |
| 01101   | ANDI rd, rs1, imm                     | AND immediate (no sign extension)   | 5 bits: 0 to 31        |
| 01110   | ORI rd, rs1, imm                      | OR immediate (no sign extension)    | 5 bits: 0 to 31        |
| 01111   | XORI rd, rs1, imm                     | XOR immediate (no sign extension)   | 5 bits: 0 to 31        |

**Immediate Value Constraints:**
- **8-bit immediates** (LDI, LUI): Use bits 7-0 of instruction, allowing larger values
- **5-bit immediates** (all others): Use bits 4-0 of instruction  
- **Sign extension**: LDI and ADDI sign-extend their immediates to 16 bits
- **No sign extension**: LUI, ANDI, ORI, XORI use immediates as unsigned values
- **Address offsets**: LDW/STW use sign-extended 5-bit offsets for base+offset addressing

#### JB-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                               |
| ------- | ------------------------------------- | ------------------------------------|
| 10000   | JMP offset                            | Unconditional jump                  |
| 10001   | JEQ offset                            | Jump if equal                       |
| 10010   | JNE offset                            | Jump if not equal                   |
| 10011   | JGT offset                            | Jump if greater than                |
| 10100   | JLT offset                            | Jump if less than                   |
| 10101   | JSR reg                               | Jump to subroutine + store pc in ra |
| 10110   | RET                                   | Return by jumping to address in ra  |

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
  LOAD16 r5, routine  ; Load routine address into r5
  JSR  r5       ; Jump to subroutine (JSR saves PC+2 to ra)
  HLT           ; Halt execution

routine:
  MOV  r1, sp   ; Copy r6 to r1
  RET           ; Return from subroutine (jumps to address in ra)
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
- **PUSH** reg ->
  - ```
    ADDI sp, sp, -2
    STW reg, [sp]
    ```
- **POP** reg ->
  - ```
    LDW reg, [sp]
    ADDI sp, sp, 2
    ```

**Stack Behavior Notes:**
- The stack grows downward from 0xCFFF (top of RAM) to 0x8000 (bottom of RAM)
- Each PUSH decrements the stack pointer by 2 bytes (16-bit word size)
- Each POP increments the stack pointer by 2 bytes  
- Always POP in reverse order of PUSH to maintain stack integrity
- Initialize the stack pointer with `LOAD16 sp, 0xCFFF` at program start

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
