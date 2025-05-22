; Fill screen with 'A' and set palette entry 0 to green

    LUI  r1, 0x40       ; r1 = 0x4000 (video memory base)
    
    LDI  r2, 0x44       ; D
    LUI  r2, 0x49       ; I

    LDI  r3, 0x2D       ; -
    LUI  r3, 0x4E       ; N
    
    LDI  r6, 0x36       ; 6
    LUI  r6, 0x31       ; 1

DRAW:
    ; Store letters at video memory location
    STW  r2, [r1+0]      
    STW  r3, [r1+2]      
    STW  r6, [r1+4]
    MOV  r2, r0
    STB  r2, [r1 + 6]      

    ; Set palette entry 0 to green (0x07E0)
    LUI  r4, 0x50         ; r4 = 0x5000 (palette memory base)
    LDI  r5, 0x07         ; High byte of 0x07E0
    LUI  r5, 0xE0         ; Low byte of 0x07E0
    STW   r5, [r4+0]      ; Store byte
    
idle:
    JMP  idle