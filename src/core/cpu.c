#include "include/cpu.h"
#include <stdio.h>
#include <string.h>

Cpu_T cpu;

Cpu_T* cpu_init(void)
{
    cpu.pc = 0;
    memset(cpu.r, 0, sizeof(cpu.r));
    cpu.cycles = 0;
    cpu.running = true;
    return &cpu;
}

uint16_t fetch_stage()
{
    return memory_read_word(cpu.pc);
}

shared reg_decode(uint16_t instruction)
{
    shared i;
    i.first = (instruction >> 8) & 0b111;
    i.second = (instruction >> 5) & 0b111;
    i.third = (instruction >> 2) & 0b111;
    uint16_t func = instruction & 0b11;
    switch ((instruction >> 11) & 0b111)
    {
    case 0x00:
        i.inst = ADD;
        break;
    case 0x01:
        i.inst = SUB;
        break;
    case 0x02:
        i.inst = AND;
        break;
    case 0x03:
        i.inst = OR;
        break;
    case 0x04:
        i.inst = XOR;
        break;
    case 0x05:
        i.inst = SHL;
        break;
    case 0x06:
        if (func == 0b00) {
            i.inst = SHR;
        } else if (func == 0b01) {
            i.inst = SRA;
        } else {
            fprintf(stderr, "Error: Invalid Instruction Used.\n");
        }
        break;
    case 0x07:
        if (func == 0b00) {
            i.inst = MOV;
        } else if (func == 0b01) {
            i.inst = CMP;
        } else if (func == 0b10) {
            i.inst = NOT;
        } else {
            fprintf(stderr, "Error: Invalid Instruction Used.\n");
        }
        break;
    default:
        fprintf(stderr, "Error: Invalid Instruction Used.\n");
        break;
    }
    return i;
}
shared imm_decode(uint16_t instruction)
{
    shared i;
    i.first = (instruction >> 8) & 0b111;
    i.second = (instruction >> 5) & 0b111;
    i.third = (instruction) & 0b11111;
    switch ((instruction >> 11) & 0b111)
    {
    case 0x00:
        i.inst = LDI;
        break;
    case 0x01:
        i.inst = LD;
        break;
    case 0x02:
        i.inst = ST;
        break;
    case 0x03:
        i.inst = ADDI;
        break;
    case 0x04:
        i.inst = SUBI;
        break;
    case 0x05:
        i.inst = ANDI;
        break;
    case 0x06:
        i.inst = ORI;
        break;
    case 0x07:
        i.inst = XORI;
        break;
    default:
        fprintf(stderr, "Error: Invalid Instruction Used.\n");
        break;
    }
    return i;
}
shared jb_decode(uint16_t instruction)
{
    shared i;
    i.first = (instruction) & 0b11111111111;
    switch ((instruction >> 11) & 0b111)
    {
    case 0x00:
        i.inst = JMP;
        break;
    case 0x01:
        i.inst = JEQ;
        break;
    case 0x02:
        i.inst = JNE;
        break;
    case 0x03:
        i.inst = JGT;
        break;
    case 0x04:
        i.inst = JLT;
        break;
    case 0x05:
        i.inst = JSR;
        break;
    case 0x06:
        i.inst = RET;
        break;
    default:
        fprintf(stderr, "Error: Invalid Instruction Used.\n");
        break;
    }
    return i;
}
shared sp_decode(uint16_t instruction)
{
    shared i;
    switch ((instruction >> 11) & 0b111)
    {
    case 0x00:
        i.inst = HLT;
        break;
    case 0x01:
        i.inst = NOP;
        break;
    case 0x02:
        i.inst = INC;
        break;
    case 0x03:
        i.inst = DEC;
        break;
    case 0x04:
        i.inst = PUSH;
        break;
    case 0x05:
        i.inst = POP;
        break;
    default:
        fprintf(stderr, "Error: Invalid Instruction Used.\n");
        break;
    }
    return i;
}
shared decode_stage(uint16_t instruction)
{
    uint8_t opcode = instruction >> 14;
    shared i;
    if (opcode <= 0x00) {
        i = reg_decode(instruction);
    } else if (opcode <= 0x01){
        i = imm_decode(instruction);
    } else if (opcode == 0x02){
        i = jb_decode(instruction);
    } else if (opcode <= 0x03){
        i = sp_decode(instruction);
    } else {
        fprintf(stderr, "Error: Invalid Instruction Used.\n");
    } 
    return i;
}