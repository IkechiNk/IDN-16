#include "idn16/cpu.h"
#include "idn16/instructions.h"
#include "idn16/dasm.h"
#include <stdio.h>
#include <string.h>
#include <time.h>

Cpu_t* cpu_init(void)
{
    // Intialize random number generator (not cryptographically secure, but sufficient for this use case)
    srand(time(NULL));

    Cpu_t* cpu = malloc(sizeof(Cpu_t));
    if (!cpu) {
        return NULL;
    }
    cpu->pc = 0;
    memset(cpu->r, 0, sizeof(cpu->r));
    memory_init(cpu->memory);
    cpu->flags.z = 0;
    cpu->flags.n = 0;
    cpu->flags.c = 0;
    cpu->flags.v = 0;
    cpu->flags.reserved = 0;
    cpu->cycles = 0;
    cpu->frame_count = 0;
    cpu->running = true;
    cpu->interrupt_pending = false;
    cpu->interrupt_type = 0;
    cpu->sleep_timer = 0;
    cpu->last_time = 0;
    return cpu;
}

void cpu_destroy(Cpu_t* cpu) {
    if (cpu) {
        free(cpu);
    }
}

void cpu_cycle(Cpu_t* cpu, bool debug) {
    if (cpu->pc > RAM_END) {
        fprintf(stderr, "Error: Program counter out of bounds: 0x%4X\n", cpu->pc);
        cpu->running = false;
        return;
    }
    uint16_t inst = fetch(cpu);
    if (debug) printf("%u: %s\n", cpu->pc / 2, disassemble_word(inst));
    execute(decode(inst), cpu);
}

uint16_t fetch(Cpu_t* cpu) {
    return memory_read_word(cpu->memory, cpu->pc);
}

shared reg_decode(uint16_t instruction) {
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
            fprintf(stderr, "Error: Invalid REG-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        }
        break;
    case 0x07:
        if (func == 0b00) {
            i.inst = MOV;
            i.third = 0;
        } else if (func == 0b01) {
            i.inst = CMP;
            i.third = 0;
        } else if (func == 0b10) {
            i.inst = NOT;
            i.third = 0;
        } else {
            fprintf(stderr, "Error: Invalid REG-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        }
        break;
    default:
        fprintf(stderr, "Error: Invalid REG-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        break;
    }
    return i;
}
shared imm_decode(uint16_t instruction) {
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
        i.inst = LDW;
        break;
    case 0x02:
        i.inst = STW;
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
        fprintf(stderr, "Error: Invalid IMM-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        break;
    }
    return i;
}
shared jb_decode(uint16_t instruction) {
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
        i.first = (instruction >> 5) & 0b111; // rs1 field for register containing target
        break;
    case 0x06:
        i.inst = RET;
        break;
    default:
        fprintf(stderr, "Error: Invalid JB-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        break;
    }
    return i;
}
shared sp_decode(uint16_t instruction) {
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
        i.inst = LDB;
        i.first = (instruction >> 8) & 0b111;
        i.second = (instruction >> 5) & 0b111;
        i.third = (instruction) & 0b11111;
        break;
    case 0x05:
        i.inst = STB;
        i.first = (instruction >> 8) & 0b111;
        i.second = (instruction >> 5) & 0b111;
        i.third = (instruction) & 0b11111;
        break;
    default:
        fprintf(stderr, "Error: Invalid SP-Instruction Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
        break;
    }
    return i;
}
shared decode(uint16_t instruction) {
    uint8_t opcode = instruction >> 14;
    shared i;
    if (opcode == 0x00) {
        i = reg_decode(instruction);
    } else if (opcode == 0x01){
        i = imm_decode(instruction);
    } else if (opcode == 0x02){
        i = jb_decode(instruction);
    } else if (opcode == 0x03){
        i = sp_decode(instruction);
    } else {
        fprintf(stderr, "Error: Invalid Opcode Used: 0x%04X -> %s.\n", instruction, disassemble_word(instruction));
    } 
    return i;
}
void execute(shared i, Cpu_t* cpu) {
    switch (i.inst) {
        case ADD:
            add(i.first, i.second, i.third, cpu);
            break;
        case SUB:
            sub(i.first, i.second, i.third, cpu);
            break;
        case AND:
            and(i.first, i.second, i.third, cpu);
            break;
        case OR:
            or(i.first, i.second, i.third, cpu);
            break;
        case XOR:
            xor(i.first, i.second, i.third, cpu);
            break;
        case SHL:
            shl(i.first, i.second, i.third, cpu);
            break;
        case SHR:
            shr(i.first, i.second, i.third, cpu);
            break;
        case SRA:
            sra(i.first, i.second, i.third, cpu);
            break;
        case MOV:
            mov(i.first, i.second, cpu);
            break;
        case CMP:
            cmp(i.first, i.second, cpu);
            break;
        case NOT:
            not(i.first, i.second, cpu);
            break;
        case LDI:
            ldi(i.first, i.second, cpu);
            break;
        case LDW:
            ldw(i.first,i.second,i.third,cpu);
            break;
        case STW:
            stw(i.first,i.second,i.third,cpu);
            break;
        case ADDI:
            addi(i.first,i.second,i.third,cpu);
            break;
        case LUI:
            lui(i.first, i.second, cpu);
            break;
        case ANDI:
            andi(i.first,i.second,i.third,cpu);
            break;
        case ORI:
            ori(i.first,i.second,i.third,cpu);
            break;
        case XORI:
            xori(i.first,i.second,i.third,cpu);
            break;
        case JMP:
            jmp(i.first,cpu);
            break;
        case JEQ:
            jeq(i.first,cpu);
            break;
        case JNE:
            jne(i.first,cpu);
            break;
        case JGT:
            jgt(i.first,cpu);
            break;
        case JLT:
            jlt(i.first,cpu);
            break;
        case JSR:
            jsr(i.first,cpu);
            break;
        case RET:
            ret(cpu);
            break;
        case HLT:
            hlt(cpu);
            break;
        case NOP:
            nop(cpu);
            break;
        case INC:
            inc(i.first, cpu);
            break;
        case DEC:
            dec(i.first, cpu);
            break;
        case LDB:
            ldb(i.first,i.second,i.third,cpu);
            break;
        case STB:
            stb(i.first,i.second,i.third,cpu);
            break;
        default:
            // fprintf(stderr, "Error: Invalid Instruction Used: 0x%04X -> %s.\n", i.inst, disassemble_word(i.inst));
            break;
    }
}

void handle_system_call(uint16_t address, Cpu_t* cpu) {
    switch (address) {
        case SYSCALL_CLEAR_SCREEN:
            syscall_clear_screen(cpu);
            break;
        case SYSCALL_PUT_CHAR:
            syscall_put_char(cpu);
            break;
        case SYSCALL_PUT_STRING:
            syscall_put_string(cpu);
            break;
        case SYSCALL_SET_CURSOR:
            syscall_set_cursor(cpu);
            break;
        case SYSCALL_GET_CURSOR:
            syscall_get_cursor(cpu);
            break;
        case SYSCALL_PUT_CHAR_AT:
            syscall_put_char_at(cpu);
            break;
        case SYSCALL_SCROLL_UP:
            syscall_scroll_up(cpu);
            break;
        case SYSCALL_FILL_AREA:
            syscall_fill_area(cpu);
            break;
        case SYSCALL_SET_TEXT_COLOR:
            syscall_set_text_color(cpu);
            break;
        case SYSCALL_GET_INPUT:
            syscall_get_input(cpu);
            break;
        case SYSCALL_PLAY_TONE:
            syscall_play_tone(cpu);
            break;
        case SYSCALL_MULTIPLY:
            syscall_multiply(cpu);
            break;
        case SYSCALL_DIVIDE:
            syscall_divide(cpu);
            break;
        case SYSCALL_RANDOM:
            syscall_random(cpu);
            break;
        case SYSCALL_MEMCPY:
            syscall_memcpy(cpu);
            break;
        case SYSCALL_PRINT_HEX:
            syscall_print_hex(cpu);
            break;
        case SYSCALL_PRINT_DEC:
            syscall_print_dec(cpu);
            break;
        case SYSCALL_SET_SPRITE:
            syscall_set_sprite(cpu);
            break;
        case SYSCALL_SET_PALETTE:
            syscall_set_palette(cpu);
            break;
        case SYSCALL_MOVE_SPRITE:
            syscall_move_sprite(cpu);
            break;
        case SYSCALL_SET_SPRITE_PIXEL:
            syscall_set_sprite_pixel(cpu);
            break;
        case SYSCALL_GET_FRAME_COUNT:
            syscall_get_frame_count(cpu);
            break;
        case SYSCALL_HIDE_SPRITE:
            syscall_hide_sprite(cpu);
            break;
        case SYSCALL_GET_SPRITE_POS:
            syscall_get_sprite_pos(cpu);
            break;
        case SYSCALL_CLEAR_SPRITE_RANGE:
            syscall_clear_sprite_range(cpu);
            break;
        case SYSCALL_CHECK_COLLISION:
            syscall_check_collision(cpu);
            break;
        case SYSCALL_SHIFT_SPRITES:
            syscall_shift_sprites(cpu);
            break;
        case SYSCALL_COPY_SPRITE:
            syscall_copy_sprite(cpu);
            break;
        case SYSCALL_SET_RETURN_ADDR:
            syscall_set_return_addr(cpu);
            break;
        case SYSCALL_MOVE_SPRITE_RIGHT:
            syscall_move_sprite_right(cpu);
            break;
        case SYSCALL_MOVE_SPRITE_LEFT:
            syscall_move_sprite_left(cpu);
            break;
        case SYSCALL_MOVE_SPRITE_UP:
            syscall_move_sprite_up(cpu);
            break;
        case SYSCALL_MOVE_SPRITE_DOWN:
            syscall_move_sprite_down(cpu);
            break;
        case SYSCALL_TIMER_START:
            syscall_timer_start(cpu);
            break;
        case SYSCALL_TIMER_QUERY:
            syscall_timer_query(cpu);
            break;
        case SYSCALL_SLEEP:
            syscall_sleep(cpu);
            break;
        case SYSCALL_NUMBER_TO_STRING:
            syscall_number_to_string(cpu);
            break;
        default:
            printf("Unknown system call: 0x%04X\n", address);
            break;
    }
}
