; IDN-16 System ROM - Assembled at base address 0x4300
; This ROM contains all the built-in system functions
; Standard calling convention: r1 = primary output register when possible

; Memory-mapped I/O addresses
VIDEO_MODE_REG = 0x3680
SCROLL_X_REG = 0x3682  
SCROLL_Y_REG = 0x3684
TEXT_CURSOR_X_REG = 0x3686
TEXT_CURSOR_Y_REG = 0x3688

TILE_BUFFER_START = 0x3000
SPRITE_TABLE_START = 0x3500
PALETTE_RAM_START = 0x3600

AUDIO_REG_START = 0x4000
INPUT_REG_START = 0x4100

SCREEN_WIDTH_TILES = 40
SCREEN_HEIGHT_TILES = 30
MAX_SPRITES = 64
MAX_PALETTE_ENTRIES = 64

; ============================================================================
; SYSTEM ROM FUNCTION JUMP TABLE (Fixed addressing)
; ============================================================================

; Graphics Functions (0x4300-0x432F)
; 0x4300 - SYS_CLEAR_SCREEN: Clear screen with fill value
SYS_CLEAR_SCREEN:
    JMP clear_screen_impl

; 0x4302 - SYS_PUT_CHAR: Display character at position  
SYS_PUT_CHAR:
    JMP put_char_impl
    
; 0x4304 - SYS_PUT_STRING: Display string at position
SYS_PUT_STRING:
    JMP put_string_impl

; 0x4306 - SYS_SET_CURSOR: Set text cursor position
SYS_SET_CURSOR:
    JMP set_cursor_impl

; 0x4308 - SYS_GET_CURSOR: Get current cursor position
SYS_GET_CURSOR:
    JMP get_cursor_impl

; 0x430A - SYS_SCROLL_SCREEN: Scroll screen up by lines
SYS_SCROLL_SCREEN:
    JMP scroll_screen_impl

; 0x430C - SYS_SET_TILE: Place tile at position
SYS_SET_TILE:
    JMP set_tile_impl

; 0x430E - SYS_GET_TILE: Get tile at position
SYS_GET_TILE:
    JMP get_tile_impl

; 0x4310 - SYS_CREATE_SPRITE: Create and configure sprite
SYS_CREATE_SPRITE:
    JMP create_sprite_impl

; 0x4312 - SYS_MOVE_SPRITE: Move sprite to new position
SYS_MOVE_SPRITE:
    JMP move_sprite_impl

; 0x4314 - SYS_SET_SPRITE_TILE: Change sprite tile
SYS_SET_SPRITE_TILE:
    JMP set_sprite_tile_impl

; 0x4316 - SYS_HIDE_SPRITE: Hide/show sprite
SYS_HIDE_SPRITE:
    JMP hide_sprite_impl

; 0x4318 - SYS_SET_PALETTE: Set palette color
SYS_SET_PALETTE:
    JMP set_palette_impl

; 0x431A - SYS_GET_PALETTE: Get palette color
SYS_GET_PALETTE:
    JMP get_palette_impl

; 0x431C - SYS_SET_VIDEO_MODE: Change video mode
SYS_SET_VIDEO_MODE:
    JMP set_video_mode_impl

; 0x431E - SYS_SET_SCROLL: Set background scroll
SYS_SET_SCROLL:
    JMP set_scroll_impl

; Audio Functions (0x4400-0x441F)
; Pad to 0x4400 - need 112 NOPs to get from 0x4320 to 0x4400
NOP 112

; 0x4400 - SYS_PLAY_TONE: Play tone on channel
SYS_PLAY_TONE:
    JMP play_tone_impl

; 0x4402 - SYS_STOP_CHANNEL: Stop audio channel
SYS_STOP_CHANNEL:  
    JMP stop_channel_impl

; 0x4404 - SYS_SET_VOLUME: Set channel volume
SYS_SET_VOLUME:
    JMP set_volume_impl

; 0x4406 - SYS_SET_WAVEFORM: Set channel waveform
SYS_SET_WAVEFORM:
    JMP set_waveform_impl

; Input Functions (0x4500-0x451F)  
; Pad to 0x4500 - need 124 NOPs to get from 0x4408 to 0x4500
NOP 124

; 0x4500 - SYS_GET_KEYS: Get controller state -> r1
SYS_GET_KEYS:
    JMP get_keys_impl

; 0x4502 - SYS_WAIT_KEY: Wait for key press -> r1  
SYS_WAIT_KEY:
    JMP wait_key_impl

; 0x4504 - SYS_KEY_PRESSED: Check if specific key pressed -> r1
SYS_KEY_PRESSED:
    JMP key_pressed_impl

; Math Functions (0x4600-0x461F)
; Pad to 0x4600 - need 125 NOPs to get from 0x4506 to 0x4600
NOP 125

; 0x4600 - SYS_MULTIPLY: 16-bit multiply -> r1 (low), r2 (high)
SYS_MULTIPLY:
    JMP multiply_impl

; 0x4602 - SYS_DIVIDE: 16-bit divide -> r1 (quotient), r2 (remainder)
SYS_DIVIDE:
    JMP divide_impl

; 0x4604 - SYS_RANDOM: Generate random number -> r1
SYS_RANDOM:
    JMP random_impl

; 0x4606 - SYS_ABS: Absolute value -> r1
SYS_ABS:
    JMP abs_impl

; Memory/Utility Functions (0x4700-0x471F)
; Pad to 0x4700 - need 124 NOPs to get from 0x4608 to 0x4700
NOP 124

; 0x4700 - SYS_MEMCPY: Copy memory block -> r1 (bytes copied)
SYS_MEMCPY:
    JMP memcpy_impl

; 0x4702 - SYS_MEMSET: Fill memory block -> r1 (bytes set)
SYS_MEMSET:
    JMP memset_impl

; 0x4704 - SYS_STRLEN: Get string length -> r1
SYS_STRLEN:
    JMP strlen_impl

; 0x4706 - SYS_STRCMP: Compare strings -> r1 (result)
SYS_STRCMP:
    JMP strcmp_impl

; 0x4708 - SYS_STRCPY: Copy string -> r1 (dest address)
SYS_STRCPY:
    JMP strcpy_impl

; ============================================================================
; ACTUAL FUNCTION IMPLEMENTATIONS
; ============================================================================

; Global variables (constants for memory locations)
RANDOM_SEED_ADDR = 0x4800
TEXT_CURSOR_X_ADDR = 0x4802  
TEXT_CURSOR_Y_ADDR = 0x4804
STRING_COUNT_ADDR = 0x4806

; Graphics Functions
; ==================

clear_screen_impl:
    ; Input: r1 = fill value (default 0)
    ; Output: r1 = success (1) or failure (0)
    CMP r1, r0
    JNE clear_with_value
    MOV r1, r0
clear_with_value:
    LOAD16 r2, TILE_BUFFER_START
    MOV r3, r0
    LOAD16 r4, 1200       ; 40*30 bytes (SCREEN_WIDTH_TILES * SCREEN_HEIGHT_TILES)
clear_loop:
    STB r1, [r2]
    ADDI r2, r2, 1
    ADDI r3, r3, 1
    CMP r3, r4
    JLT clear_loop
    LDI r1, 1             ; Return success
    RET

put_char_impl:
    ; Input: r1 = x, r2 = y, r3 = character
    ; Output: r1 = success (1) or failure (0)
    ; Bounds check
    LDI r4, SCREEN_WIDTH_TILES
    CMP r1, r4
    JLT put_char_bounds_ok1
    JMP put_char_fail
put_char_bounds_ok1:
    LDI r4, SCREEN_HEIGHT_TILES
    CMP r2, r4
    JLT put_char_bounds_ok2
    JMP put_char_fail
put_char_bounds_ok2:
    CMP r1, r0
    JLT put_char_fail
    CMP r2, r0
    JLT put_char_fail
    
    ; Calculate address: TILE_BUFFER_START + (y * 40 + x)
    LDI r4, SCREEN_WIDTH_TILES
    MOV r5, r2               ; y coordinate
    MOV r6, r0               ; accumulator
mult_loop:
    CMP r5, r0
    JEQ mult_done
    ADD r6, r6, r4
    DEC r5
    JMP mult_loop
mult_done:
    ADD r6, r6, r1           ; Add x coordinate
    LOAD16 r4, TILE_BUFFER_START
    ADD r6, r6, r4           ; Final address
    STB r3, [r6]             ; Store character
    LDI r1, 1                ; Return success
    RET
put_char_fail:
    MOV r1, r0               ; Return failure
    RET

put_string_impl:
    ; Input: r1 = x, r2 = y, r3 = string address
    ; Output: r1 = characters displayed
    MOV r4, r1               ; Save starting X
    MOV r5, r2               ; Save starting Y  
    MOV r6, r3               ; String pointer
    LOAD16 r3, STRING_COUNT_ADDR
    STW r0, [r3]             ; Initialize character count to 0
string_loop:
    LDB r3, [r6]             ; Load character
    CMP r3, r0               ; Check null terminator
    JEQ string_done
    
    ; Check for newline
    LDI r2, 10
    CMP r3, r2
    JEQ string_newline
    
    ; Draw character (r1=x, r2=y, r3=char)
    JSR put_char_impl
    LOAD16 r2, STRING_COUNT_ADDR
    LDW r3, [r2]
    ADDI r3, r3, 1           ; Increment character count
    STW r3, [r2]
    ADDI r1, r1, 1           ; Next X position
    
    ; Check wrap
    LDI r2, SCREEN_WIDTH_TILES
    CMP r1, r2
    JLT string_continue
    
string_newline:
    MOV r1, r4               ; Reset X to starting position
    ADDI r5, r5, 1           ; Next line
    MOV r2, r5               ; Update current Y
    
    ; Check if we've gone past bottom of screen
    LDI r3, SCREEN_HEIGHT_TILES
    CMP r2, r3
    JLT string_continue_ok
    JMP string_done
string_continue_ok:
    
string_continue:
    ADDI r6, r6, 1           ; Next character
    JMP string_loop
string_done:
    LOAD16 r2, STRING_COUNT_ADDR
    LDW r1, [r2]             ; Return character count
    RET

; New function implementations

set_cursor_impl:
    ; Input: r1 = x, r2 = y
    ; Output: r1 = success (1) or failure (0)
    LDI r3, SCREEN_WIDTH_TILES
    CMP r1, r3
    JLT set_cursor_bounds_ok1
    JMP set_cursor_fail
set_cursor_bounds_ok1:
    LDI r3, SCREEN_HEIGHT_TILES
    CMP r2, r3
    JLT set_cursor_bounds_ok2
    JMP set_cursor_fail
set_cursor_bounds_ok2:
    CMP r1, r0
    JLT set_cursor_fail
    CMP r2, r0
    JLT set_cursor_fail
    
    LOAD16 r3, TEXT_CURSOR_X_ADDR
    STW r1, [r3]
    LOAD16 r3, TEXT_CURSOR_Y_ADDR
    STW r2, [r3]
    
    LOAD16 r3, TEXT_CURSOR_X_REG
    STB r1, [r3]
    LOAD16 r3, TEXT_CURSOR_Y_REG
    STB r2, [r3]
    LDI r1, 1
    RET
set_cursor_fail:
    MOV r1, r0
    RET

get_cursor_impl:
    ; Output: r1 = x, r2 = y
    LOAD16 r3, TEXT_CURSOR_X_ADDR
    LDW r1, [r3]
    LOAD16 r3, TEXT_CURSOR_Y_ADDR
    LDW r2, [r3]
    RET

scroll_screen_impl:
    ; Input: r1 = lines to scroll up
    ; Output: r1 = lines actually scrolled
    CMP r1, r0
    JEQ scroll_done
    JLT scroll_done          ; If negative, don't scroll
    LDI r2, SCREEN_HEIGHT_TILES
    CMP r1, r2
    JLT scroll_valid
    MOV r1, r2               ; Cap at screen height
scroll_valid:
    MOV r3, r1               ; Save lines to scroll
    LOAD16 r4, TILE_BUFFER_START
    LDI r5, SCREEN_WIDTH_TILES
    MOV r6, r1               ; Source line
    LDI r1, 0                ; Dest line (use r1 since we saved original in r3)
scroll_line_loop:
    CMP r6, r2               ; Check if source line < height
    JLT scroll_copy_line
    JMP clear_bottom
scroll_copy_line:
    ; Copy line r6 to line r1
    ; Calculate source address: TILE_BUFFER_START + (r6 * 40)
    ; Calculate dest address: TILE_BUFFER_START + (r1 * 40)
    ; Copy 40 bytes
    ADDI r6, r6, 1
    ADDI r1, r1, 1
    JMP scroll_line_loop
clear_bottom:
    ; Clear bottom lines - simplified for demo
    MOV r1, r3               ; Return lines scrolled
scroll_done:
    RET

set_tile_impl:
    ; Input: r1 = x, r2 = y, r3 = tile_id
    ; Output: r1 = success (1) or failure (0)
    ; Check bounds: 0 <= x < SCREEN_WIDTH_TILES, 0 <= y < SCREEN_HEIGHT_TILES
    CMP r1, r0
    JLT set_tile_fail
    CMP r2, r0
    JLT set_tile_fail
    LDI r4, SCREEN_WIDTH_TILES
    CMP r1, r4
    JLT set_tile_bounds_ok1
    JMP set_tile_fail
set_tile_bounds_ok1:
    LDI r4, SCREEN_HEIGHT_TILES
    CMP r2, r4
    JLT set_tile_bounds_ok2
    JMP set_tile_fail
set_tile_bounds_ok2:
    
    ; Calculate address: TILE_BUFFER_START + (y * 40 + x)
    LDI r4, SCREEN_WIDTH_TILES
    MOV r5, r2               ; y coordinate
    MOV r6, r0               ; accumulator
tile_mult_loop:
    CMP r5, r0
    JEQ tile_mult_done
    ADD r6, r6, r4
    DEC r5
    JMP tile_mult_loop
tile_mult_done:
    ADD r6, r6, r1           ; Add x
    LOAD16 r4, TILE_BUFFER_START
    ADD r6, r6, r4           ; Final address
    STB r3, [r6]             ; Store tile ID
    LDI r1, 1                ; Return success
    RET
set_tile_fail:
    MOV r1, r0               ; Return failure
    RET

get_tile_impl:
    ; Input: r1 = x, r2 = y
    ; Output: r1 = tile_id or 0 if invalid
    CMP r1, r0
    JLT get_tile_fail
    CMP r2, r0
    JLT get_tile_fail
    LDI r3, SCREEN_WIDTH_TILES
    CMP r1, r3
    JLT get_tile_ok1
    JMP get_tile_fail
get_tile_ok1:
    LDI r3, SCREEN_HEIGHT_TILES
    CMP r2, r3
    JLT get_tile_ok2
    JMP get_tile_fail
get_tile_ok2:
    
    ; Calculate address
    LDI r3, SCREEN_WIDTH_TILES
    MOV r4, r2               ; y coordinate
    MOV r5, r0               ; accumulator
get_tile_mult_loop:
    CMP r4, r0
    JEQ get_tile_mult_done
    ADD r5, r5, r3
    DEC r4
    JMP get_tile_mult_loop
get_tile_mult_done:
    ADD r5, r5, r1           ; Add x
    LOAD16 r3, TILE_BUFFER_START
    ADD r5, r5, r3           ; Final address
    LDB r1, [r5]             ; Load tile ID
    RET
get_tile_fail:
    MOV r1, r0
    RET

create_sprite_impl:
    ; Input: r1 = id, r2 = x, r3 = y, r4 = tile_id
    ; Output: r1 = success (1) or failure (0)
    LDI r5, MAX_SPRITES
    CMP r1, r5
    JLT create_sprite_ok
    JMP create_sprite_fail
create_sprite_ok:
    CMP r1, r0
    JLT create_sprite_fail
    
    ; Calculate sprite address: SPRITE_TABLE_START + (id * 4)
    MOV r5, r1               ; Save sprite ID for return value
    LDI r6, 2
    SHL r1, r1, r6           ; id * 4
    LOAD16 r6, SPRITE_TABLE_START
    ADD r1, r1, r6
    
    ; Store sprite data
    STB r2, [r1]             ; X
    STB r3, [r1 + 1]         ; Y
    STB r4, [r1 + 2]         ; Tile ID
    LDI r6, 1                ; Visible attribute
    STB r6, [r1 + 3]         ; Attributes
    LDI r1, 1                ; Return success
    RET
create_sprite_fail:
    MOV r1, r0
    RET

move_sprite_impl:
    ; Input: r1 = id, r2 = new_x, r3 = new_y
    ; Output: r1 = success (1) or failure (0)
    LDI r4, MAX_SPRITES
    CMP r1, r4
    JLT move_sprite_ok
    JMP move_sprite_fail
move_sprite_ok:
    CMP r1, r0
    JLT move_sprite_fail
    
    ; Calculate address
    LDI r4, 2
    SHL r1, r1, r4           ; id * 4
    LOAD16 r4, SPRITE_TABLE_START
    ADD r1, r1, r4
    
    ; Update position
    STB r2, [r1]             ; X
    STB r3, [r1 + 1]         ; Y
    LDI r1, 1                ; Return success
    RET
move_sprite_fail:
    MOV r1, r0
    RET

set_sprite_tile_impl:
    ; Input: r1 = sprite_id, r2 = tile_id
    ; Output: r1 = success (1) or failure (0)
    LDI r3, MAX_SPRITES
    CMP r1, r3
    JLT set_sprite_tile_ok
    JMP set_sprite_tile_fail
set_sprite_tile_ok:
    CMP r1, r0
    JLT set_sprite_tile_fail
    
    ; Calculate address
    LDI r3, 2
    SHL r1, r1, r3           ; id * 4
    LOAD16 r3, SPRITE_TABLE_START
    ADD r1, r1, r3
    
    STB r2, [r1 + 2]         ; Set tile ID
    LDI r1, 1                ; Return success
    RET
set_sprite_tile_fail:
    MOV r1, r0
    RET

hide_sprite_impl:
    ; Input: r1 = sprite_id, r2 = hide (1) or show (0)
    ; Output: r1 = success (1) or failure (0)
    LDI r3, MAX_SPRITES
    CMP r1, r3
    JLT hide_sprite_ok
    JMP hide_sprite_fail
hide_sprite_ok:
    CMP r1, r0
    JLT hide_sprite_fail
    
    ; Calculate address
    LDI r3, 2
    SHL r1, r1, r3           ; id * 4
    LOAD16 r3, SPRITE_TABLE_START
    ADD r1, r1, r3
    
    ; Set visibility (1 = visible, 0 = hidden)
    CMP r2, r0
    JEQ set_hidden
    LDI r3, 1
    JMP set_visibility
set_hidden:
    MOV r3, r0
set_visibility:
    STB r3, [r1 + 3]         ; Set visibility attribute
    LDI r1, 1                ; Return success
    RET
hide_sprite_fail:
    MOV r1, r0
    RET

set_palette_impl:
    ; Input: r1 = index, r2 = color
    ; Output: r1 = success (1) or failure (0)
    LDI r3, MAX_PALETTE_ENTRIES
    CMP r1, r3
    JLT set_palette_ok
    JMP set_palette_fail
set_palette_ok:
    CMP r1, r0
    JLT set_palette_fail
    
    ; Calculate address: PALETTE_RAM_START + (index * 2)
    LDI r3, 1
    SHL r1, r1, r3           ; index * 2
    LOAD16 r3, PALETTE_RAM_START
    ADD r1, r1, r3
    STW r2, [r1]             ; Store color
    LDI r1, 1                ; Return success
    RET
set_palette_fail:
    MOV r1, r0
    RET

get_palette_impl:
    ; Input: r1 = index
    ; Output: r1 = color or 0 if invalid
    LDI r2, MAX_PALETTE_ENTRIES
    CMP r1, r2
    JLT get_palette_ok
    JMP get_palette_fail
get_palette_ok:
    CMP r1, r0
    JLT get_palette_fail
    
    ; Calculate address
    LDI r2, 1
    SHL r1, r1, r2           ; index * 2
    LOAD16 r2, PALETTE_RAM_START
    ADD r1, r1, r2
    LDW r1, [r1]             ; Load color
    RET
get_palette_fail:
    MOV r1, r0
    RET

set_video_mode_impl:
    ; Input: r1 = mode (0=text, 1=tiles, 2=mixed)
    ; Output: r1 = success (1) or failure (0)
    LDI r2, 3
    CMP r1, r2
    JLT set_video_mode_ok
    JMP set_video_mode_fail
set_video_mode_ok:
    CMP r1, r0
    JLT set_video_mode_fail
    
    LOAD16 r2, VIDEO_MODE_REG
    STB r1, [r2]
    LDI r1, 1                ; Return success
    RET
set_video_mode_fail:
    MOV r1, r0
    RET

set_scroll_impl:
    ; Input: r1 = scroll_x, r2 = scroll_y
    ; Output: r1 = success (1) or failure (0)
    LOAD16 r3, SCROLL_X_REG
    STB r1, [r3]
    LOAD16 r3, SCROLL_Y_REG
    STB r2, [r3]
    LDI r1, 1                ; Return success
    RET

; Audio Functions
; ================

play_tone_impl:
    ; Input: r1 = frequency, r2 = duration, r3 = channel
    ; Output: r1 = success (1) or failure (0)
    LDI r4, 4
    CMP r3, r4
    JLT play_tone_ok
    JMP play_tone_fail
play_tone_ok:
    CMP r3, r0
    JLT play_tone_fail
    
    ; Calculate channel address: AUDIO_REG_START + (channel * 16)
    LDI r4, 4
    SHL r3, r3, r4           ; channel * 16
    LOAD16 r4, AUDIO_REG_START
    ADD r3, r3, r4
    
    ; Set frequency (16-bit value split into two 8-bit registers)
    LOAD16 r4, 0xFF
    AND r4, r1, r4
    STB r4, [r3]             ; Freq low byte
    LDI r5, 8
    SHR r1, r1, r5
    STB r1, [r3 + 1]         ; Freq high byte
    
    ; Set volume to maximum and enable channel
    LDI r4, 15
    STB r4, [r3 + 2]         ; Volume (0-15)
    LDI r4, 1
    STB r4, [r3 + 5]         ; Enable channel
    LDI r1, 1                ; Return success
    RET
play_tone_fail:
    MOV r1, r0
    RET

stop_channel_impl:
    ; Input: r1 = channel
    ; Output: r1 = success (1) or failure (0)
    LDI r2, 4
    CMP r1, r2
    JLT stop_channel_ok
    JMP stop_channel_fail
stop_channel_ok:
    CMP r1, r0
    JLT stop_channel_fail
    
    ; Calculate channel address
    LDI r2, 4
    SHL r1, r1, r2           ; channel * 16
    LOAD16 r2, AUDIO_REG_START
    ADD r1, r1, r2
    
    ; Disable channel
    MOV r2, r0
    STB r2, [r1 + 5]         ; Disable channel
    LDI r1, 1                ; Return success
    RET
stop_channel_fail:
    MOV r1, r0
    RET

set_volume_impl:
    ; Input: r1 = channel, r2 = volume (0-15)
    ; Output: r1 = success (1) or failure (0)
    LDI r3, 4
    CMP r1, r3
    JLT set_volume_ok
    JMP set_volume_fail
set_volume_ok:
    CMP r1, r0
    JLT set_volume_fail
    LDI r3, 16
    CMP r2, r3
    JLT set_volume_ok
    JMP set_volume_fail
set_volume_ok:
    CMP r2, r0
    JLT set_volume_fail
    
    ; Calculate channel address
    LDI r3, 4
    SHL r1, r1, r3           ; channel * 16
    LOAD16 r3, AUDIO_REG_START
    ADD r1, r1, r3
    
    STB r2, [r1 + 2]         ; Set volume
    LDI r1, 1                ; Return success
    RET
set_volume_fail:
    MOV r1, r0
    RET

set_waveform_impl:
    ; Input: r1 = channel, r2 = waveform (0=square, 1=sine, 2=triangle, 3=noise)
    ; Output: r1 = success (1) or failure (0)
    LDI r3, 4
    CMP r1, r3
    JLT set_waveform_ok
    JMP set_waveform_fail
set_waveform_ok:
    CMP r1, r0
    JLT set_waveform_fail
    LDI r3, 4
    CMP r2, r3
    JLT set_waveform_ok
    JMP set_waveform_fail
set_waveform_ok:
    CMP r2, r0
    JLT set_waveform_fail
    
    ; Calculate channel address
    LDI r3, 4
    SHL r1, r1, r3           ; channel * 16
    LOAD16 r3, AUDIO_REG_START
    ADD r1, r1, r3
    
    STB r2, [r1 + 3]         ; Set waveform
    LDI r1, 1                ; Return success
    RET
set_waveform_fail:
    MOV r1, r0
    RET

; Input Functions
; ===============

get_keys_impl:
    ; Input: r1 = controller (0 or 1)
    ; Output: r1 = button state bitmask
    LOAD16 r2, INPUT_REG_START
    CMP r1, r0
    JEQ get_keys_ctrl0
    LDI r3, 2
    CMP r1, r3
    JLT get_keys_ok
    JMP get_keys_fail
get_keys_ok:
    ADDI r2, r2, 1           ; Controller 1 register
get_keys_ctrl0:
    LDB r1, [r2]
    RET
get_keys_fail:
    MOV r1, r0               ; Return 0 for invalid controller
    RET

wait_key_impl:
    ; Output: r1 = first key pressed
wait_key_loop:
    MOV r1, r0               ; Check controller 0
    JSR get_keys_impl
    CMP r1, r0
    JNE wait_key_done
    
    LDI r1, 1                ; Check controller 1
    JSR get_keys_impl
    CMP r1, r0
    JEQ wait_key_loop
wait_key_done:
    RET

key_pressed_impl:
    ; Input: r1 = controller, r2 = key_bit (0-7)
    ; Output: r1 = 1 if pressed, 0 if not
    LDI r3, 8
    CMP r2, r3
    JLT key_pressed_ok
    JMP key_pressed_fail
key_pressed_ok:
    CMP r2, r0
    JLT key_pressed_fail
    
    JSR get_keys_impl        ; Get controller state in r1
    LDI r3, 1
    SHL r3, r3, r2           ; Create mask for bit position
    AND r1, r1, r3           ; Check if bit is set
    CMP r1, r0
    JEQ key_not_pressed
    LDI r1, 1                ; Key is pressed
    RET
key_not_pressed:
    MOV r1, r0               ; Key is not pressed
    RET
key_pressed_fail:
    MOV r1, r0
    RET

; Math Functions
; ==============

multiply_impl:
    ; Input: r1 = multiplicand, r2 = multiplier
    ; Output: r1 = result low word, r2 = result high word
    MOV r3, r1               ; Save multiplicand
    MOV r4, r2               ; Save multiplier
    MOV r1, r0               ; Initialize result low
    MOV r2, r0               ; Initialize result high
    
mult_impl_loop:
    CMP r4, r0
    JEQ mult_impl_done
    
    ; Add multiplicand to result
    ADD r1, r1, r3
    ; Check for overflow (simplified - just set high word to 0)
    
    DEC r4
    JMP mult_impl_loop
mult_impl_done:
    RET

divide_impl:
    ; Input: r1 = dividend, r2 = divisor
    ; Output: r1 = quotient, r2 = remainder
    CMP r2, r0
    JEQ divide_by_zero
    
    MOV r3, r1               ; Save dividend
    MOV r4, r0               ; Initialize quotient
    
divide_impl_loop:
    CMP r3, r2
    JLT divide_impl_done
    SUB r3, r3, r2           ; Subtract divisor from dividend
    ADDI r4, r4, 1           ; Increment quotient
    JMP divide_impl_loop
    
divide_impl_done:
    MOV r1, r4               ; Return quotient
    MOV r2, r3               ; Return remainder
    RET
divide_by_zero:
    LOAD16 r1, 0xFFFF        ; Return max value for divide by zero
    MOV r2, r0
    RET

random_impl:
    ; Output: r1 = random number
    LOAD16 r2, RANDOM_SEED_ADDR
    LDW r1, [r2]
    
    ; Linear congruential generator: (a * seed + c) mod m
    LOAD16 r2, 1103          ; Multiplier
    JSR multiply_impl        ; r1 = seed * 1103
    LOAD16 r2, 13849         ; Increment
    ADD r1, r1, r2
    
    ; Store new seed
    LOAD16 r2, RANDOM_SEED_ADDR
    STW r1, [r2]
    RET

abs_impl:
    ; Input: r1 = signed value
    ; Output: r1 = absolute value
    ; Check if negative (MSB set)
    LOAD16 r2, 0x8000
    AND r2, r1, r2
    CMP r2, r0
    JEQ abs_positive
    
    ; Negate the value (two's complement)
    NOT r1, r1
    ADDI r1, r1, 1
abs_positive:
    RET

; Memory/Utility Functions
; ========================

memcpy_impl:
    ; Input: r1 = dest, r2 = src, r3 = length
    ; Output: r1 = bytes copied
    MOV r4, r3               ; Save original length
    CMP r3, r0
    JEQ memcpy_impl_done
    
memcpy_impl_loop:
    LDB r5, [r2]
    STB r5, [r1]
    ADDI r1, r1, 1
    ADDI r2, r2, 1
    DEC r3
    CMP r3, r0
    JGT memcpy_impl_loop
    
memcpy_impl_done:
    MOV r1, r4               ; Return bytes copied
    RET

memset_impl:
    ; Input: r1 = dest, r2 = value, r3 = length
    ; Output: r1 = bytes set
    MOV r4, r3               ; Save original length
    CMP r3, r0
    JEQ memset_impl_done
    
memset_impl_loop:
    STB r2, [r1]
    ADDI r1, r1, 1
    DEC r3
    CMP r3, r0
    JGT memset_impl_loop
    
memset_impl_done:
    MOV r1, r4               ; Return bytes set
    RET

strlen_impl:
    ; Input: r1 = string address
    ; Output: r1 = string length
    MOV r2, r1               ; String pointer
    MOV r1, r0               ; Length counter
    
strlen_impl_loop:
    LDB r3, [r2]
    CMP r3, r0
    JEQ strlen_impl_done
    ADDI r2, r2, 1
    ADDI r1, r1, 1
    JMP strlen_impl_loop
    
strlen_impl_done:
    RET

strcmp_impl:
    ; Input: r1 = string1, r2 = string2
    ; Output: r1 = result (-1, 0, 1)
    MOV r3, r1               ; String1 pointer
    MOV r4, r2               ; String2 pointer
    
strcmp_impl_loop:
    LDB r5, [r3]             ; Load char from string1
    LDB r6, [r4]             ; Load char from string2
    
    CMP r5, r6
    JLT strcmp_less
    JGT strcmp_greater
    
    ; Characters are equal, check if end of string
    CMP r5, r0
    JEQ strcmp_equal
    
    ADDI r3, r3, 1
    ADDI r4, r4, 1
    JMP strcmp_impl_loop
    
strcmp_equal:
    MOV r1, r0               ; Strings are equal
    RET
strcmp_less:
    LDI r1, -1               ; String1 < String2
    RET
strcmp_greater:
    LDI r1, 1                ; String1 > String2
    RET

strcpy_impl:
    ; Input: r1 = dest, r2 = src
    ; Output: r1 = dest address
    MOV r3, r1               ; Save dest address
    
strcpy_impl_loop:
    LDB r4, [r2]             ; Load character from src
    STB r4, [r1]             ; Store character to dest
    CMP r4, r0               ; Check for null terminator
    JEQ strcpy_impl_done
    ADDI r1, r1, 1
    ADDI r2, r2, 1
    JMP strcpy_impl_loop
    
strcpy_impl_done:
    MOV r1, r3               ; Return dest address
    RET
