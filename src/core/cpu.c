#include "cpu.h"
#include "instructions.h"
#include <stdio.h>
#include <string.h>

Cpu_T cpu;

Cpu_T* cpu_init(void)
{
    cpu.pc = 0;
    memset(cpu.r, 0, sizeof(cpu.r));
    memory_init(cpu.memory);
    cpu.flags.z = 0;
    cpu.flags.n = 0;
    cpu.flags.c = 0;
    cpu.flags.v = 0;
    cpu.flags.i = 0;
    cpu.flags.reserved = 0;
    cpu.cycles = 0;
    cpu.running = true;
    return &cpu;
}

uint16_t fetch_stage()
{
    return memory_read_word(cpu.memory, cpu.pc);
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
        i.second = instruction & 0b11111111;
        i.third = 0;
        break;
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
        i.inst = LUI;
        i.second = instruction & 0b11111111;
        i.third = 0;
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
        i.first = (instruction >> 8) & 0b111;
        break;
    case 0x03:
        i.inst = DEC;
        i.first = (instruction >> 8) & 0b111;
        break;
    case 0x04:
        i.inst = PUSH;
        i.first = (instruction >> 8) & 0b111;
        break;
    case 0x05:
        i.inst = POP;
        i.first = (instruction >> 8) & 0b111;
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
void execute_stage(shared i) {
    switch (i.inst) {
        case ADD:
            add(i.first, i.second, i.third, &cpu);
            break;
        case SUB:
            sub(i.first, i.second, i.third, &cpu);
            break;
        case AND:
            and(i.first, i.second, i.third, &cpu);
            break;
        case OR:
            or(i.first, i.second, i.third, &cpu);
            break;
        case XOR:
            xor(i.first, i.second, i.third, &cpu);
            break;
        case SHL:
            shl(i.first, i.second, i.third, &cpu);
            break;
        case SHR:
            shr(i.first, i.second, i.third, &cpu);
            break;
        case SRA:
            sra(i.first, i.second, i.third, &cpu);
            break;
        case MOV:
            mov(i.first, i.second, &cpu);
            break;
        case CMP:
            cmp(i.first, i.second, &cpu);
            break;
        case NOT:
            not(i.first, i.second, &cpu);
            break;
        case LDI:
            ldi(i.first, (i.third), &cpu);
            break;
        case LD:
            ld(i.first,i.second,i.third,&cpu);
            break;
        case ST:
            st(i.first,i.second,i.third,&cpu);
            break;
        case ADDI:
            addi(i.first,i.second,i.third,&cpu);
            break;
        case LUI:
            lui(i.first, i.second, &cpu);
            break;
        case ANDI:
            andi(i.first,i.second,i.third,&cpu);
            break;
        case ORI:
            ori(i.first,i.second,i.third,&cpu);
            break;
        case XORI:
            xori(i.first,i.second,i.third,&cpu);
            break;
        case JMP:
            jmp(i.first,&cpu);
            break;
        case JEQ:
            jeq(i.first,&cpu);
            break;
        case JNE:
            jne(i.first,&cpu);
            break;
        case JGT:
            jgt(i.first,&cpu);
            break;
        case JLT:
            jlt(i.first,&cpu);
            break;
        case JSR:
            jsr(i.first,&cpu);
            break;
        case RET:
            ret(&cpu);
            break;
        case HLT:
            hlt(&cpu);
            break;
        case NOP:
            nop(&cpu);
            break;
        case INC:
            inc(i.first, &cpu);
            break;
        case DEC:
            dec(i.first, &cpu);
            break;
        case PUSH:
            push(i.first, &cpu);
            break;
        case POP:
            pop(i.first, &cpu);
            break;
        default:
            fprintf(stderr, "Error: Invalid Instruction Used.\n");
            break;
    }
}
