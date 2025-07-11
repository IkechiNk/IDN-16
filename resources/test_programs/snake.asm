; Simple Snake Game for IDN-16
; A minimal snake game with WASD controls

; =============================================================================
; MEMORY ADDRESSES AND CONSTANTS
; =============================================================================

; Generic constants
STARTING_SIZE = 0


; Hardware addresses
PALETTE_RAM_START = 0xD4D0
SPRITE_TABLE_START = 0xD4F0
APPLE_SPRITE = 0xD4F0
SNAKE_HEAD_SPRITE = 0xD4F3
TILESET_DATA_START = 0xE300

; Game state (using RAM area)
SNAKE_X = 0x8000                    ; Snake head X position
SNAKE_Y = 0x8001                    ; Snake head Y position
SNAKE_DIR_ADDR = 0x8002             ; Snake direction (0=right, 1=down, 2=left, 3=up)
SNAKE_LEN = 0x8003                  ; Snake length
APPLE_X = 0x8005                    ; Apple X position
APPLE_Y = 0x8006                    ; Apple Y position
APPLE_ACTIVE = 0x8007               ; Apple spawn state
TEMP_X = 0x8008                     ; Holder Y for body moving
TEMP_Y = 0x8009                     ; Holder Y for body moving
GAME_OVER_TEXT = 0x800A             ; Game over tex tstring location
SCORE_TEXT = 0x8015                 ; Score text string location
SCORE_TEXT_NUMBER_START = 0x801C    ; Score number string location

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
GREEN_BODY_TILE = 2
RED_APPLE_TILE = 3

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
SYSCALL_SET_TILE_PIXEL_COLOR = 0xF314
SYSCALL_SET_SPRITE = 0xF311
SYSCALL_CHECK_COLLISION	= 0xF319
SYSCALL_PUT_CHAR = 0xF301
SYSCALL_PUT_STRING = 0xF302
SYSCALL_SET_CURSOR = 0xF303
SYSCALL_NUMBER_TO_STRING = 0xF324


; =============================================================================
; MAIN PROGRAM
; =============================================================================

start:    
    ; Initialize game state
    LOAD16 r1, SNAKE_X
    LDI r2, 20
    STB r2, [r1]
    LOAD16 r1, SNAKE_Y
    LDI r2, 15
    STB r2, [r1]
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
    ; Create snake head tile (tile 1) - green square
    LDI r1, GREEN_HEAD_TILE
    LDI r2, 0           ; X
    LDI r3, 0           ; Y
    LDI r4, 10          ; Green palette index
    LOAD16 r5, 0xF314   ; SYSCALL_SET_TILE_PIXEL_COLOR
    JSR r5
    
    LDI r2, 1           ; X
    LDI r3, 3           ; Y
    LDI r4, 0           ; black palette index
    JSR r5
    LDI r2, 1           ; X
    LDI r3, 4           ; Y
    LDI r4, 0           ; black palette index
    JSR r5
    LDI r2, 6           ; X
    LDI r3, 3           ; Y
    LDI r4, 0           ; black palette index
    JSR r5
    LDI r2, 5           ; X
    LDI r3, 4           ; Y
    LDI r4, 0           ; black palette index
    JSR r5
    LDI r2, 2           ; X
    LDI r3, 3           ; Y
    LDI r4, 10          ; Green palette index
    JSR r5
    LDI r2, 3           ; X
    LDI r3, 4           ; Y
    LDI r4, 10          ; Green palette index
    JSR r5
    LDI r2, 7           ; X
    LDI r3, 3           ; Y
    LDI r4, 10          ; Green palette index
    JSR r5
    LDI r2, 7           ; X
    LDI r3, 4           ; Y
    LDI r4, 10          ; Green palette index
    JSR r5

    LDI r1, SNAKE_HEAD_INDEX
    LDI r2, 0
    LDI r3, 0
    LDI r4, GREEN_HEAD_TILE
    LOAD16 r5, SYSCALL_SET_SPRITE
    JSR r5

    ; Create snake body tile (tile 2) - green square
    LDI r1, GREEN_BODY_TILE
    LDI r2, 0           ; X
    LDI r3, 0           ; Y
    LDI r4, 10          ; Green palette index
    LOAD16 r5, 0xF314   ; SYSCALL_SET_TILE_PIXEL_COLOR
    JSR r5

    LDI r1, SNAKE_HEAD_INDEX
    LDI r2, 0
    LDI r3, 0
    LDI r4, GREEN_BODY_TILE
    LOAD16 r5, SYSCALL_SET_SPRITE
    JSR r5

    ; Create apple tile (tile 3) - red square
    LDI r1, RED_APPLE_TILE
    LDI r2, 0          ; X
    LDI r3, 0          ; Y
    LDI r4, 12         ; Red palette index
    LOAD16 r5, SYSCALL_SET_TILE_PIXEL_COLOR
    JSR r5
    
    LDI r1, APPLE_INDEX
    LDI r2, 10
    LDI r3, 10
    LDI r4, RED_APPLE_TILE
    LOAD16 r5, SYSCALL_SET_SPRITE
    JSR r5

init_game_over:
    LOAD16 r2, GAME_OVER_TEXT
    LDI r1, 'G'
    STB r1, [r2+0] 
    LDI r1, 'A'
    STB r1, [r2+1] 
    LDI r1, 'M'
    STB r1, [r2+2] 
    LDI r1, 'E'
    STB r1, [r2+3] 

    LDI r1, ' '
    STB r1, [r2+4] 

    LDI r1, 'O'
    STB r1, [r2+5] 
    LDI r1, 'V'
    STB r1, [r2+6] 
    LDI r1, 'E'
    STB r1, [r2+7] 
    LDI r1, 'R'
    STB r1, [r2+8]
    STB r0, [r2+9]

init_score:
    LOAD16 r2, SCORE_TEXT
    LDI r1, 'S'
    STB r1, [r2]
    LDI r1, 'C'
    STB r1, [r2+1]
    LDI r1, 'O'
    STB r1, [r2+2]
    LDI r1, 'R'
    STB r1, [r2+3]
    LDI r1, 'E'
    STB r1, [r2+4]
    LDI r1, ':'
    STB r1, [r2+5]
    LDI r1, ' '
    STB r1, [r2+6]
    LDI r1, 0
    STB r1, [r2+7]

LOAD16 r1, spawn_apple
JSR r1 

main:
    LOAD16 r1, check_input
    JSR r1

    LOAD16 r1, update_sprites
    JSR r1

    LOAD16 r1, update_score
    JSR r1

    LOAD16 r1, self_collision_check
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

    PUSH r2
    PUSH r3

    ; Temporary value storage
    LOAD16 r1, SNAKE_HEAD_SPRITE
    LDB r2, [r1]    ; Head x
    LDB r3, [r1+1]  ; Head y
    LOAD16 r1, TEMP_X
    STB r2, [r1]
    LOAD16 r1, TEMP_Y
    STB r3, [r1]
    
    POP r3
    POP r2

    ; Update snake head sprite (sprite 0)
    LDI r1, SNAKE_HEAD_INDEX          ; Snake_sprite
    LDI r4, GREEN_HEAD_TILE
    LOAD16 r5, 0xF311       ; SET_SPRITE syscall
    JSR r5

    JMP move_body

    JMP func_done

move_body:
    PUSH ra

    ; Loop setup
    LOAD16 r1, SNAKE_HEAD_SPRITE ; Sprite pointer
    LOAD16 r5, SNAKE_LEN    ; Counter
    LDW r5, [r5]
    LDI r4, 1
    CMP r5, r4
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

    LDI r2, GREEN_BODY_TILE  ; Change later to body tile
    STB r2, [r1+2]  ; Active body sprite with tile

    LOAD16 r2, TEMP_Y
    STB r3, [r2]    ; Store prev y in next slot

    ; Loop maintanance
    ADDI r5, r5, -1
    LDI r4, 1
    CMP r5, r4
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
    JLT game_over
    LDI r5, SCREEN_WIDTH
    ADDI r5, r5, -1
    CMP r2, r5
    JGT game_over
    CMP r3, r0
    JLT game_over
    LDI r5, SCREEN_HEIGHT
    ADDI r5, r5, -1
    CMP r3, r5
    JGT game_over
    
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

self_collision_check:
    PUSH ra
    
    LDI r1, SNAKE_HEAD_INDEX   ; Head index
    MOV r2, r1                 ; Body counter
    
    LOAD16 r4, SNAKE_LEN
    LDW r4, [r4+0] ; Max counter value
    
self_col:
    CMP r2, r4
    JEQ func_done

    LDI r1, SNAKE_HEAD_INDEX   ; Head index
    ADDI r2, r2, 1
    LOAD16 r3, SYSCALL_CHECK_COLLISION
    JSR r3
    CMP r1, r0
    JEQ self_col

    JMP game_over

game_over:
    LOAD16 r3, SYSCALL_SET_CURSOR
    LDI r1, 14
    LDI r2, 15
    JSR r3

    LOAD16 r3, SYSCALL_PUT_STRING
    LOAD16 r1, GAME_OVER_TEXT
    LOAD16 r2, 10
    JSR r3
    JMP game_over

update_score:
    PUSH ra

    LOAD16 r5, SYSCALL_NUMBER_TO_STRING
    LOAD16 r1, SNAKE_LEN
    
    LDW r1, [r1]
    ADDI r1, r1, -1                     ; r1 = score
    LOAD16 r2, SCORE_TEXT_NUMBER_START  ; r2 = write pointer
    LDI r3, 5                           ; r3 = max_size
    LDI r4, 10                          ; r4 = base 10
    JSR r5

    LOAD16 r5, SYSCALL_SET_CURSOR
    LDI r1, 0
    LDI r2, 0
    JSR r5

    LOAD16 r5, SYSCALL_PUT_STRING
    LOAD16 r1, SCORE_TEXT
    LDI r2, 10
    JSR r5

    JMP func_done
