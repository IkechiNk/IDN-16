; Simple Snake Game for IDN-16
; A minimal snake game with WASD controls

; =============================================================================
; MEMORY ADDRESSES AND CONSTANTS
; =============================================================================

; Generic constants
STARTING_SIZE = 1000


; Hardware addresses
PALETTE_RAM_START = 0xD4D0
SPRITE_TABLE_START = 0xD4F0
APPLE_SPRITE = 0xD4F0
SNAKE_HEAD_SPRITE = 0xD4F3
TILESET_DATA_START = 0xE300

; Game state (using RAM area)
SNAKE_X = 0x8000            ; Snake head X position
SNAKE_Y = 0x8001            ; Snake head Y position
SNAKE_DIR_ADDR = 0x8002     ; Snake direction (0=right, 1=down, 2=left, 3=up)
SNAKE_LEN = 0x8003          ; Snake length
APPLE_X = 0x8005            ; Apple X position
APPLE_Y = 0x8006            ; Apple Y position
APPLE_ACTIVE = 0x8007       ; Apple spawn state
TEMP_X = 0x8008             ; Holder Y for body moving
TEMP_Y = 0x8009             ; Holder Y for body moving

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

; Sprite Indicies
APPLE_INDEX = 0
SNAKE_HEAD_INDEX = 1

; Tile Indicies
GREEN_HEAD_TILE = 1
RED_APPLE_TILE = 2

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
SYSCALL_RANDOM = 0xF30D
SYSCALL_DIVIDE = 0xF30C
SYSCALL_HIDE_SPRITE	= 0xF316
SYSCALL_SET_SPRITE_PIXEL = 0xF314
SYSCALL_SET_SPRITE = 0xF311
SYSCALL_CHECK_COLLISION	= 0xF319

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
    LDI r2, DIR_RIGHT        ; Start moving right
    STB r2, [r1]
    LOAD16 r1, SNAKE_LEN
    LOAD16 r2, STARTING_SIZE ; spawn_apple will increment by 1 before main loop
    STW r2, [r1]
    LOAD16 r1, APPLE_ACTIVE
    LDI r2, 0
    STB r2, [r1]

init_graphics:
    ; Create snake tile (tile 1) - green square
    LDI r1, GREEN_HEAD_TILE
    LDI r2, 0           ; X
    LDI r3, 0           ; Y
    LDI r4, 10          ; Green palette index
    LOAD16 r5, 0xF314   ; SYSCALL_SET_SPRITE_PIXEL
    JSR r5

    LDI r1, SNAKE_HEAD_INDEX
    LDI r2, 0
    LDI r3, 0
    LDI r4, GREEN_HEAD_TILE
    LOAD16 r5, SYSCALL_SET_SPRITE
    JSR r5

    ; Create apple tile (tile 2) - red square
    LDI r1, RED_APPLE_TILE
    LDI r2, 0          ; X
    LDI r3, 0          ; Y
    LDI r4, 12         ; Red palette index
    LOAD16 r5, SYSCALL_SET_SPRITE_PIXEL
    JSR r5
    
    LDI r1, APPLE_INDEX
    LDI r2, 10
    LDI r3, 10
    LDI r4, RED_APPLE_TILE
    LOAD16 r5, SYSCALL_SET_SPRITE
    JSR r5

LOAD16 r1, spawn_apple
JSR r1 

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
    
    LDI r3, DIR_DOWN
    LDB r4, [r1]
    CMP r3, r4
    JEQ func_done 
    
    LDI r2, DIR_UP
    STB r2, [r1]
    JMP func_done

    
check_s:
    ; Check S (down)
    LDI r2, INPUT_S
    AND r3, r1, r2
    CMP r3, r0
    JEQ check_a
    LOAD16 r1, SNAKE_DIR_ADDR

    LDI r3, DIR_UP
    LDB r4, [r1]
    CMP r3, r4
    JEQ func_done

    LDI r2, DIR_DOWN
    STB r2, [r1]
    JMP func_done

    
check_a:
    ; Check A (left)
    LDI r2, INPUT_A
    AND r3, r1, r2
    CMP r3, r0
    JEQ check_d
    LOAD16 r1, SNAKE_DIR_ADDR

    LDI r3, DIR_RIGHT
    LDB r4, [r1]
    CMP r3, r4
    JEQ func_done

    LDI r2, DIR_LEFT
    STB r2, [r1]
    JMP func_done

    
check_d:
    ; Check D (right)
    LDI r2, INPUT_D
    AND r3, r1, r2
    CMP r3, r0
    JEQ func_done
    LOAD16 r1, SNAKE_DIR_ADDR

    LDI r3, DIR_LEFT
    LDB r4, [r1]
    CMP r3, r4
    JEQ func_done

    LDI r2, DIR_RIGHT
    STB r2, [r1]
    JMP func_done

func_done:
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
    
    JMP func_done
query_timer:
    PUSH ra

    LOAD16 r2, SYSCALL_TIMER_QUERY
    LDI r1, 0
    JSR r2

    JMP func_done
sleep:
    PUSH ra

    LOAD16 r2, SYSCALL_SLEEP
    JSR r2
    
    JMP func_done

update_sprites:
    PUSH ra

    ; If timer is active, don't update sprites
    JMP query_timer
    CMP r1, r0
    JGT func_done
    JMP set_timer

    LOAD16 r1, check_eat
    JSR r1

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
    
    ; Update snake head sprite (sprite 0)
    LDI r1, SNAKE_HEAD_INDEX          ; Snake_sprite
    LDI r4, GREEN_HEAD_TILE           ; Snake head tile
    LOAD16 r5, 0xF311       ; SET_SPRITE syscall
    JSR r5

    JMP move_body

    JMP func_done

move_body:
    PUSH ra
        
    LOAD16 r1, SNAKE_HEAD_SPRITE
    LDB r2, [r1]    ; Head x
    LDB r3, [r1+1]  ; Head y
    LOAD16 r1, TEMP_X
    STB r2, [r1]
    LOAD16 r1, TEMP_Y
    STB r3, [r1]

    LOAD16 r1, SNAKE_HEAD_SPRITE ; Sprite pointer
    LOAD16 r5, SNAKE_LEN    ; Counter
    LDW r5, [r5]
    CMP r5, r0
    JEQ func_done
loop:
    ADDI r1, r1, 3

    LOAD16 r2, TEMP_X
    LDB r2, [r2]    ; Next x
    
    LDB r3, [r1]    ; Curr x

    STB r2, [r1]    ; Store next x in cur slot

    LOAD16 r2, TEMP_X
    STB r3, [r2]    ; Store prev x in next slot

    LOAD16 r2, TEMP_Y
    LDB r2, [r2]    ; Next y
    
    LDB r3, [r1+1]  ; Curr y

    STB r2, [r1+1]  ; Store next y in cur slot

    LDI r2, GREEN_HEAD_TILE  ; Change later to body tile
    STB r2, [r1+2]  ; Active body sprite with tile

    LOAD16 r2, TEMP_Y
    STB r3, [r2]    ; Store prev y in next slot

    ADDI r5, r5, -1
    CMP r5, r0
    JGT loop
    
    JMP func_done

move_right:
    PUSH ra

    LOAD16 r1, SNAKE_X
    LDB r2, [r1]
    ADDI r2, r2, 1

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]

    JMP check_wall_collision
    
    JMP func_done
move_down:
    PUSH ra

    LOAD16 r1, SNAKE_X
    LDB r2, [r1]

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]
    ADDI r3, r3, 1
    
    JMP check_wall_collision

    JMP func_done
move_left:
    PUSH ra
    
    LOAD16 r1, SNAKE_X
    LDB r2, [r1]
    ADDI r2, r2, -1

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]

    JMP check_wall_collision
    
    JMP func_done
move_up:
    PUSH ra

    LOAD16 r1, SNAKE_X
    LDB r2, [r1]

    LOAD16 r1, SNAKE_Y
    LDB r3, [r1]
    ADDI r3, r3, -1
    
    JMP check_wall_collision
    
    JMP func_done

check_wall_collision:
    PUSH ra

    ; Check wall collision
    CMP r2, r0
    JLT func_done
    LDI r5, SCREEN_WIDTH
    ADDI r5, r5, -1
    CMP r2, r5
    JGT func_done
    CMP r3, r0
    JLT func_done
    LDI r5, SCREEN_HEIGHT
    ADDI r5, r5, -1
    CMP r3, r5
    JGT func_done
    
    ; Update snake position
    LOAD16 r1, SNAKE_X
    STB r2, [r1]
    LOAD16 r1, SNAKE_Y
    STB r3, [r1]
    
    JMP func_done

spawn_apple:
    PUSH ra

    LOAD16 r1, SNAKE_LEN
    LDW r2, [r1]
    addi r2, r2, 1
    STW r2, [r1]

    ; Random Y
    LOAD16 r1, SYSCALL_RANDOM
    JSR r1
    LOAD16 r3, SYSCALL_DIVIDE
    LDI r2, 28
    JSR r3
    
    ADDI r2, r2, 1
    LOAD16 r1, APPLE_Y
    STB r2, [r1]

    ; Random X
    LOAD16 r1, SYSCALL_RANDOM
    JSR r1
    LOAD16 r3, SYSCALL_DIVIDE
    LDI r2, 38
    JSR r3
    
    ADDI r2, r2, 1
    LOAD16 r1, APPLE_X
    STB r2, [r1]

    LOAD16 r5, SYSCALL_SET_SPRITE
    LDI r1, APPLE_INDEX
    LOAD16 r2, APPLE_X
    LDB r2, [r2]
    LOAD16 r3, APPLE_Y
    LDB r3, [r3]
    LDI r4, RED_APPLE_TILE
    JSR r5

    JMP func_done

check_eat:
    PUSH ra

    LOAD16 r3, SYSCALL_CHECK_COLLISION
    LDI r1, SNAKE_HEAD_INDEX
    LDI r2, APPLE_INDEX
    JSR r3
    CMP r1, r0
    JNE spawn_apple
    JMP func_done
