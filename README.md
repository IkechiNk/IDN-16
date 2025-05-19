# `IDN-16` Console

## Table of Contents


- [`IDN-16` Console](#idn-16-console)
  - [Table of Contents](#table-of-contents)
  - [About](#about)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Installing](#installing)
  - [Documentation](#documentation)
    - [Memory Mapping](#memory-mapping)
    - [IO-Interfacing](#io-interfacing)
      - [Input System (0x7000-0x70FF)](#input-system-0x7000-0x70ff)
      - [Sound System (0x6000-0x60FF)](#sound-system-0x6000-0x60ff)
      - [System Control (0x8000-0x80FF)](#system-control-0x8000-0x80ff)
    - [CPU Architecture](#cpu-architecture)
      - [Registers](#registers)
      - [Status Flags](#status-flags)
    - [Instruction Set](#instruction-set)
      - [Instruction Formats](#instruction-formats)
    - [Specific Opcodes and categories](#specific-opcodes-and-categories)
      - [REG-Format Instructions](#reg-format-instructions)
      - [IMM-Format Instructions](#imm-format-instructions)
      - [JB-Format Instructions](#jb-format-instructions)
      - [SP-Format Instructions](#sp-format-instructions)
    - [Assembly Language](#assembly-language)
      - [Syntax Overview](#syntax-overview)
      - [Addressing Modes](#addressing-modes)
      - [Psuedo-Instructions](#psuedo-instructions)

## About

`IDN-16` is a 16-bit, simulated console featuring a custom CPU architecture, memory-mapped I/O, and a risc-v inspited custom instruction set. This serves as a way for me to solidify what I learned about low-level computer architecture, emulation techniques, and graphics programming, while creating something that I find fun and creatively satisfying.

## Getting Started

These instructions will get you a copy of the project up and running on your local machine for development and testing purposes.

### Prerequisites

You will need the following software installed:

- `GCC` or `Clang` (C compiler)
- `Make`
- `SDL2` development libraries (for input handling)
- `Git` (to clone the repository)

For Ubuntu/Debian, these can be installed by running the following:
```
sudo apt update 
sudo apt install build-essential libsdl2-dev git
```

### Installing

**Clone the repository**
```
git clone https://github.com/yourusername/`IDN-16`.git cd `IDN-16`
```
**Build the project**
```
make
```
 **Prepare a ROM file**
Place your compiled ROM as `rom.bin` in the project root directory. The emulator loads this file at startup.
**Run the emulator**
```
./idn16
```

## Documentation

### Memory Mapping
The `IDN-16` has a 64KB byte-addressable memory space sectioned into several dedicated regions:
```
0x0000-0x1FFF: ROM (8KB program storage, read-only)
0x2000-0x3FFF: RAM (8KB general purpose, read-write)
0x4000-0x47FF: Video memory (2KB for display buffer)
0x4800-0x4FFF: Sprite data (2KB for sprite definitions)
0x5000-0x50FF: Palette data (256 bytes for color definitions)
0x6000-0x60FF: Sound registers (256 bytes)
0x7000-0x70FF: Input registers (256 bytes)
0x8000-0x80FF: System control (256 bytes for interrupts, timers)
0xF000-0xFFFF: System ROM (4KB for built-in functions)
```
### IO-Interfacing
The IK-16 uses memory-mapped I/O for interfacing with specific I/O:

#### Input System (0x7000-0x70FF)
**0x7000**: Controller 1 button state (read-only)
- Bit 0: A button
- Bit 1: B button
- Bit 2: Start
- Bit 3: Select
- Bit 4: Up
- Bit 5: Down
- Bit 6: Left
- Bit 7: Right

**0x7001**: Controller 2 button state (read-only)

#### Sound System (0x6000-0x60FF)
For each channel N (0-3), starting at 0x6000 + N*16:
- **0x6N00**: Frequency low byte (read/write)
- **0x6N01**: Frequency high byte (read/write)
- **0x6N02**: Volume (read/write)
- **0x6N03**: Waveform select (read/write)

#### System Control (0x8000-0x80FF)
- **0x8000**: System status register
- **0x8001**: Interrupt control register

### CPU Architecture
#### Registers
The `IDN-16`'s CPU has 8 16-bit registers:
| Register      | Purpose         |
| ------------- | --------------- |
| r0 / zero     | General / Zero  |
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
- Interrupt Enable (I): Controls interrupt processing

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

| Opcode  | Assembly Syntax                       | Usage                           |
| ------- | ------------------------------------- | ------------------------------- |
| 00000   | ADD rd, rs1, rs2                      | Add registers                   |
| 00001   | SUB rd, rs1, rs2                      | Subtract registers              |
| 00010   | AND rd, rs1, rs2                      | Bitwise AND                     |
| 00011   | OR rd, rs1, rs2                       | Bitwise OR                      |
| 00100   | XOR rd, rs1, rs2                      | Bitwise XOR                     |
| 00101   | SHL rd, rs1, rs2                      | Shift left                      |
| 00110   | SHR rd, rs1, rs2 (func=00)            | Shift right logical             |
| 00110   | SRA rd, rs1, rs2 (func=01)            | Shift right arithmetic          |
| 00111   | MOV rd, rs1 (func=00)                 | Copy register                   |
| 00111   | CMP rd, rs1, rs2 (func=01)            | Compare regs and set flags      |
| 00111   | NOT rd, rs1 (func=10)                 | Bitwise NOT                     |

#### IMM-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                           |
| ------- | ------------------------------------- | ------------------------------- |
| 01000   | LDI rd, imm                           | Load immediate                  |
| 01001   | LD rd, [rs1+imm]                      | Load word from memory           |
| 01010   | ST [rd+imm], rs1                      | Store word to memory            |
| 01011   | ADDI rd, rs1, imm                     | Add immediate                   |
| 01100   | SUBI rd, rs1, imm                     | Subtract immediate              |
| 01101   | ANDI rd, rs1, imm                     | AND immediate                   |
| 01110   | ORI rd, rs1, imm                      | OR immediate                    |
| 01111   | XORI rd, rs1, imm                     | XOR immediate                   |

#### JB-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                           |
| ------- | ------------------------------------- | ------------------------------- |
| 10000   | JMP offset                            | Unconditional jump              |
| 10001   | JEQ offset                            | Jump if equal                   |
| 10010   | JNE offset                            | Jump if not equal               |
| 10011   | JGT offset                            | Jump if greater than            |
| 10100   | JLT offset                            | Jump if less than               |
| 10101   | JSR offset                            | Jump to subroutine              |
| 10110   | RET                                   | Return from subroutine          |

#### SP-Format Instructions

| Opcode  | Assembly Syntax                       | Usage                           |
| ------- | ------------------------------------- | ------------------------------- |
| 11000   | HLT                                   | Halt processor                  |
| 11001   | NOP                                   | No operation                    |
| 11010   | INC rd                                | Increment register              |
| 11011   | DEC rd                                | Decrement register              |
| 11100   | PUSH rd                               | Push register to stack          |
| 11101   | POP rd                                | Pop from stack to register      |
| 11110   | LUI rd, imm                           | Load immediate to upper 8 bits  |

### Assembly Language
#### Syntax Overview
```
; This is a comment
label:          ; Define a label
  LDI  r0, 10   ; Load immediate value 10 into r0
  JSR  routine  ; Jump to subroutine "routine"
  HLT           ; Halt execution

routine:
  MOV  r1, r0   ; Copy r0 to r1
  RET           ; Return from subroutine
```
#### Addressing Modes
- **Immediate:** LDI r0, 5
- **Register:** MOV r0, r1
- **Register Indirect with Offset:** LD r0, [r1+2]
#### Psuedo-Instructions
The assembler has support for some pseudo-instructions:
- **NOP** → Expands to **MOV** r0, r0
- **CALL** addr → Expands to **JSR** addr
---
This documentation will be expanded as development continues.