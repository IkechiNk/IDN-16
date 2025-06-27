; Simple Snake Game for IDN-16
; A minimal snake game with WASD controls

; =============================================================================
; MEMORY ADDRESSES AND CONSTANTS
; =============================================================================

; Hardware addresses
PALETTE_RAM_START = 0xD4D0
SPRITE_TABLE_START = 0xD4F0
TILESET_DATA_START = 0xD5B0

; Game state (using RAM area)
SNAKE_X = 0x8000            ; Snake head X position
SNAKE_Y = 0x8001            ; Snake head Y position
SNAKE_DIR_ADDR = 0x8002     ; Snake direction (0=right, 1=down, 2=left, 3=up)
SNAKE_LEN = 0x8003          ; Snake length
APPLE_X = 0x8004            ; Apple X position
APPLE_Y = 0x8005            ; Apple Y position

; Input constants
INPUT_W = 16                ; Up (W key) - bit 4
INPUT_S = 32                ; Down (S key) - bit 5  
INPUT_A = 64                ; Left (A key) - bit 6
INPUT_D = 128               ; Right (D key) - bit 7
INPUT_F = 1                 ; F key (button A) - bit 0

; Direction constants
DIR_RIGHT = 0
DIR_DOWN = 1
DIR_LEFT = 2
DIR_UP = 3

; Screen size
SCREEN_WIDTH = 40
SCREEN_HEIGHT = 30

; System calling
SYSCALL_MOVE_SPRITE_RIGHT = 0xF31D
SYSCALL_MOVE_SPRITE_LEFT = 0xF31E
SYSCALL_MOVE_SPRITE_UP = 0xF31F
SYSCALL_MOVE_SPRITE_DOWN = 0xF320
SYSCALL_TIMER_START = 0xF321
SYSCALL_TIMER_QUERY = 0xF322
SYSCALL_SLEEP = 0xF323

; =============================================================================
; MAIN PROGRAM
; =============================================================================

start:
    ; Initialize stack pointer
    LOAD16 sp, 0xCFFF
    
    ; Initialize game state
    LOAD16 r1, SNAKE_X
    STB r0, [r1]
    LOAD16 r1, SNAKE_Y
    STB r0, [r1]
    LOAD16 r1, SNAKE_DIR_ADDR
    LDI r2, DIR_RIGHT       ; Start moving right
    STB r2, [r1]
    LOAD16 r1, SNAKE_LEN
    LDI r2, 1               ; Start with length 1
    STB r2, [r1]

init_graphics:
    ; Set up color palette
    ; Create snake tile (tile 1) - green square
    LDI r1, 1           ; tile 1
    LDI r2, 0           ; X
    LDI r3, 0           ; Y
    LDI r4, 10          ; Green palette index
    LOAD16 r5, 0xF314   ; SYSCALL_SET_SPRITE_PIXEL
    JSR r5

    LDI r1, 0
    LDI r2, 0
    LDI r3, 0
    LDI r4, 1
    LOAD16 r5, 0xF311   ; SYSCALL_SET_SPRITE
    JSR r5

main:
    LOAD16 r1, check_input
    JSR r1

    LOAD16 r1, update_sprites
    JSR r1

    LOAD16 r1, main
    JSR r1

check_input:
    ; Store ra
    PUSH ra

    ; Handle input
    LDI r1, 0
    LOAD16 r2, 0xF309       ; GET_INPUT syscall
    JSR r2
    JMP check_w

check_w:
    ; Check W (up)    
    LDI r2, INPUT_W
    AND r3, r1, r2
    CMP r3, r0
    JEQ check_s
    LOAD16 r1, SNAKE_DIR_ADDR
    LDI r2, DIR_UP
    STB r2, [r1]
    JMP input_done

    
check_s:
    ; Check S (down)
    LDI r2, INPUT_S
    AND r3, r1, r2
    CMP r3, r0
    JEQ check_a
    LOAD16 r1, SNAKE_DIR_ADDR
    LDI r2, DIR_DOWN
    STB r2, [r1]
    JMP input_done

    
check_a:
    ; Check A (left)
    LDI r2, INPUT_A
    AND r3, r1, r2
    CMP r3, r0
    JEQ check_d
    LOAD16 r1, SNAKE_DIR_ADDR
    LDI r2, DIR_LEFT
    STB r2, [r1]
    JMP input_done

    
check_d:
    ; Check D (right)
    LDI r2, INPUT_D
    AND r3, r1, r2
    CMP r3, r0
    JEQ input_done
    LOAD16 r1, SNAKE_DIR_ADDR
    LDI r2, DIR_RIGHT
    STB r2, [r1]
    JMP input_done

input_done:
    POP ra
    RET

set_timer:
    PUSH ra

    LOAD16 r2, SYSCALL_TIMER_QUERY
    LDI r1, 1
    JSR r2

    LOAD16 r2, SYSCALL_TIMER_START
    LDI r1, 17
    JSR r2
    
    POP ra
    RET
query_timer:
    PUSH ra

    LOAD16 r2, SYSCALL_TIMER_QUERY
    LDI r1, 0
    JSR r2

    POP ra
    RET
sleep:
    PUSH ra

    LOAD16 r2, SYSCALL_SLEEP
    JSR r2
    
    POP ra
    RET

update_sprites:
    PUSH ra

    ; If timer is active, don't move sprite
    JMP query_timer
    CMP r1, r0
    JGT input_done
    JMP set_timer
    
    ; Get current direction
    LOAD16 r1, SNAKE_DIR_ADDR
    LDB r1, [r1]

    LOAD16 r2, DIR_RIGHT
    CMP r1, r2
    JEQ move_right
    
    LOAD16 r2, DIR_DOWN
    CMP r1, r2
    JEQ move_down

    LOAD16 r2, DIR_LEFT
    CMP r1, r2
    JEQ move_left

    LOAD16 r2, DIR_UP
    CMP r1, r2
    JEQ move_up

    ; Move sprite
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]            ; Snake X
    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]            ; Snake Y
    
    ; Update snake sprite (sprite 0)
    MOV r1, r0              ; Sprite 0
    LDI r4, 1               ; Tile 1 (snake)
    LOAD16 r5, 0xF311       ; SET_SPRITE syscall
    JSR r5

    POP ra
    RET

move_right:
    PUSH ra
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]
    ADDI r2, r2, 1

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]

    JMP check_collision
    
    LOAD16 r4, 0xFFF0

    JMP input_done
move_down:
    PUSH ra
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]
    ADDI r3, r3, 1
    
    JMP check_collision

    LOAD16 r4, 0xFFF1

    JMP input_done
move_left:
    PUSH ra
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]
    ADDI r2, r2, -1

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]

    JMP check_collision
    
    LOAD16 r4, 0xFFF2

    JMP input_done
move_up:
    PUSH ra
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]
    ADDI r3, r3, -1
    
    JMP check_collision
    
    LOAD16 r4, 0xFFF3

    JMP input_done

check_collision:
    ; Check wall collision
    CMP r2, r0
    JLT input_done
    LDI r5, SCREEN_WIDTH
    ADDI r5, r5, -1
    CMP r2, r5
    JGT input_done
    CMP r3, r0
    JLT input_done
    LDI r5, SCREEN_HEIGHT
    ADDI r5, r5, -1
    CMP r3, r5
    JGT input_done
    
    ; Update snake position
    LOAD16 r1, SNAKE_X
    STB r2, [r1]
    LOAD16 r1, SNAKE_Y
    STB r3, [r1]

    RET
