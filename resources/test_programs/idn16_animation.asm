; IDN-16 Animated Logo Display
; This program displays "IDN-16" with simple animation effects

; Constants
DELAY_COUNT = 30000
CHAR_I = 73
CHAR_D = 68  
CHAR_N = 78
CHAR_DASH = 45
CHAR_1 = 49
CHAR_6 = 54
CHAR_SPACE = 32
SYSCALL_PUT_CHAR = 0xF301
SYSCALL_CLEAR_SCREEN = 0xF300

start:
    ; Clear the screen first
    LDI r1, CHAR_SPACE          ; Fill with space characters
    JSR 0xF300                  ; SYSCALL_CLEAR_SCREEN
    
    ; Set up starting position (center of 40x30 screen)
    LDI r4, 17                  ; Starting X position (center for 6 chars)
    LDI r5, 15                  ; Starting Y position (center)
    
    ; Animation loop - display characters one by one
    JSR display_i
    JSR delay
    
    JSR display_d  
    JSR delay
    
    JSR display_n
    JSR delay
    
    JSR display_dash
    JSR delay
    
    JSR display_1
    JSR delay
    
    JSR display_6
    JSR delay
    
    ; Blink effect - clear and redisplay 3 times
    LDI r7, 3                   ; Blink counter
    
blink_loop:
    ; Clear the text area
    JSR clear_text_area
    JSR delay
    
    ; Redisplay all text
    JSR display_all
    JSR delay
    
    DEC r7
    CMP r7, r0
    JNE blink_loop
    
    ; Final display and halt
    JSR display_all
    HLT

; Display individual characters
display_i:
    LDI r1, CHAR_I
    MOV r2, r4                  ; X position
    MOV r3, r5                  ; Y position  
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1              ; Move to next position
    RET

display_d:
    LDI r1, CHAR_D
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1
    RET

display_n:
    LDI r1, CHAR_N
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1
    RET

display_dash:
    LDI r1, CHAR_DASH
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1
    RET

display_1:
    LDI r1, CHAR_1
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1
    RET

display_6:
    LDI r1, CHAR_6
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    RET

; Display all characters at once
display_all:
    LDI r4, 17                  ; Reset X position
    JSR display_i
    JSR display_d
    JSR display_n
    JSR display_dash
    JSR display_1
    JSR display_6
    RET

; Clear just the text area
clear_text_area:
    LDI r6, 6                   ; 6 characters to clear
    LDI r4, 17                  ; Reset X position
    
clear_loop:
    LDI r1, CHAR_SPACE
    MOV r2, r4
    MOV r3, r5
    JSR SYSCALL_PUT_CHAR
    ADDI r4, r4, 1
    DEC r6
    CMP r6, r0
    JNE clear_loop
    RET

; Simple delay function
delay:
    LOAD16 r6, DELAY_COUNT      ; Load delay counter
delay_loop:
    DEC r6
    CMP r6, r0
    JNE delay_loop
    RET