struct Key {
    enum KeyEnum : int {
        Nokey = 0, Bell = 7, BackSpace = 8, Tab = 9, LF = 10, FF = 12, CR = 13, Enter = CR, Escape = 27 /*0x1b,033*/,
        Space = 0x20, At = 0x40, A = 0x41, Z = 0x5a, a = 0x61, z = 0x7a,
        Nil = 0x100, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        ScrollLock = 0x110, Pause, Ins, Del, CapsLock,
        ShiftL = 0x120, ShiftR, ControlL, WinL, AltL, AltR, WinR, Menu, ControlR,
        Home = 0x130, Left, Up, Right, Down, PgUp, PgDn, End, //FirstFnKey = F1, LastFnKey = 0x1cf,
        MouseMove = 0x1d0, MouseLeft = 0x1d1, MouseMiddle, MouseRight, MousePgUp, MousePgDn, MouseX1, MouseX2,
        /*Release*/ MouseRelLeft = 0x1e1, MouseRelMiddle, MouseRelRight, MouseRelPgUp, MouseRelPgDn, MouseRelX1, MouseRelX2,
        NextFrame = 0x1ff, MAX = 0x1ff, SIZE = 0x200
    };
    enum StateEnum : int {
        StateShift =		0x01,
        StateCapsLock =		0x02,
        StateControl =		0x04,
        StateAlt =		0x08,
        StateMod2 =		0x10,
        StateMod3 =		0x20,
        StateMod4 =		0x40,
        StateWin =		0x80,
        StateMouseLeft =	0x100,
        StateMouseMiddle =	0x200,
        StateMouseRight =	0x400,
        StateMousePgUp =	0x800,
        StateMousePgDn =	0x1000,
        StateLang1 =		0x2000,
        StateLang2 =		0x4000,
    };
};

extern "C" {
    const int FB_WIDTH = 800, FB_HEIGHT = 600, FB_STRIDE = FB_WIDTH * sizeof(unsigned);
    typedef unsigned int framebuf_t[FB_HEIGHT][FB_WIDTH];
    extern framebuf_t *pframebuf;
    //     _________   _________   _______
    // | |/   11    \ /   11    \ /   9   \  = size
    // |3|3         2|1        10|0       0| = bit-
    // |1|09876543210|98765432109|876543210|   number
    // |-|     y     |     x     |   key   | = field
    int getkey(void);
    int waitkey(void);
    void asm_main_text(void);
}

void asm_main_text(void)
{
    asm volatile (R"(
        {.intel_syntax noprefix | }
        call asm_main
        jmp asm_exit
        .global asm_main

    .section data1, "awx"

#.=0040d000
dot: .int .

    .align 16

pic:
    .int 0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xff444444,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00
    .int 0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00,0xffffff00
star:
    .incbin "star.bmp", 54, 32*32*3

hello:  .string "dot=xxxxxxxx"
hex:    .string "0123456789abcdef"

    .align 8
asm_main:
    # 260,400
    mov eax, 400
    .byte 0xb8, 0x90, 1, 0, 0
    mov ebx, 260
    call PutPic16x24

    #340,400
    mov eax, 400
    mov ebx, 340
    call PutPic16x24

    #260,500
    mov eax, 500
    mov ebx, 260
    call PutPic16x24

    #340,500
    mov eax, 500
    mov ebx, 340
    call PutPic16x24

/*
    # clear screen
    push eax
    xor eax, eax
    mov ecx, 800*600
    mov edi, [pframebuf]
    rep stosd
    pop eax
*/
    mov eax, 100
    mov ebx, 140
    mov ecx, 0xff3388
    mov edx, 'A'
    call PutChar

    mov edx, 400
    mov edi, offset hello+4
    mov ecx, 8
1:  # 0x12345678
    rol edx, 4
    # 0x23456781
    mov eax, edx
    and eax, 0x0f
    # 1 (00.0f)
    mov al, [hex+eax]
    stosb # al -> [edi], edi++
    loop 1b

    mov eax, 300
    mov ebx, 160
    mov ecx, 0xff3388
    mov edx, offset hello
    call PutStr

    //     _________   _________   _______
    // | |/   11    \ /   11    \ /   9   \  = size
    // |3|3         2|1        10|0      00| = bit-
    // |1|09876543210|98765432109|876543210|   number
    // |-|     y     |     x     |   key   | = field

    // MouseMove event:
    //  0 yyyyyyyyyyy xxxxxxxxxxx 111010000 (0x1d0)
next:
    call getkey
    mov ebx, eax
    and ebx, 0b111111111
    cmp ebx, 0x1d0 # MouseMove
    jne 9f

    mov ebx, eax		# extract x
    and eax, 0b00000000000011111111111000000000
    shr eax, 9
    mov edx, 800-16	# clamp x to 0..800-16
    cmp eax, edx
    cmova eax, edx
                        # extract y
    and ebx, 0b01111111111100000000000000000000
    shr ebx, 20
    mov edx, 600-24	# clamp y to 0..600-24
    cmp ebx, edx
    cmova ebx, edx
    // eax - x, ebx - y
    call PutPic16x24

9:  cmp ebx, 0x1ff # NextFrame
    jne next

    mov eax, 300
    mov ebx, 160
    mov ecx, 0xff3388
    mov edx, offset hello
    call PutStr

    call animate
    jmp next

    jmp endpic


animate:
    xor eax, eax
    mov ecx, 800*32
    mov edi, [pframebuf]
    add edi, 800*500 * 4
    rep stosd
    mov eax, [animate_x]
    mov ebx, 500
    call PutStar
    cmp eax, 800 - 32
    jb 1f
    neg dword ptr [animate_v]
1:  add eax, [animate_v]
    mov [animate_x], eax
    ret

animate_x:
    .int 0
animate_v:
    .int 1

PutStar:
    pusha
    imul edi, ebx, 800
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov esi, offset star
    mov ecx, 32
1:  push ecx
    mov ecx, 32
    xor eax, eax
2:  lodsb al, [esi] # esi++
    ror eax, 8
    lodsb al, [esi] # esi++
    ror eax, 8
    lodsb al, [esi] # esi++
    ror eax, 16
    cmp eax, 0xffffff
    je 3f
    # stosd es:[edi], eax # edi+=4
    mov [edi], eax
3:  add edi, 4
    loop 2b
    pop ecx
    add edi, (800 - 32) * 4
    loop 1b
    popa
    ret

#
# void PutPic16x24(int (edi) offset_in_pframebuff_in_bytes)
# void PutPic16x24(int (eax) x, (ebx) y)
#

PutPic16x24:
    imul edi, ebx, 800
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov esi, offset pic
    mov ecx, 24
1:  call PutLine16
    add edi, (800 - 16) * 4
    loop 1b
    ret

PutLine16:
    push ecx
    mov ecx, 16
    rep movsd [edi], [esi]
    pop ecx
    ret

ClearScreen:
    push eax
    xor eax, eax
    mov ecx, 800*600
    mov edi, [pframebuf]
    rep stosd
    pop eax
    ret

# void PutChar(int (eax) x, (ebx) y, (ecx) color, (edx) ch)
PutChar:
    push ebp
    mov ebp, esp

    push edx        # [ebp-4] = esp -> [edx]  ebp -> [oldebp]
    push ecx
    push ebx
    push eax

    # x=[ebp-16] y=[ebp-12] color=[ebp-8] ch=[ebp-4]
    sub esp, 8

    mov ebx, offset font8x13
    mov eax, [ebp - 4] # ch
    imul eax, eax, 13
    add ebx, eax
    mov edi, [pframebuf]
    add edi, [ebp - 16] # x
    mov eax, [ebp - 12] # y
    imul eax, eax, 800
    add edi, eax

    mov ecx, 13
    putlet:
        push ecx
        mov ecx, 8
        mov al, [ebx]

    putline:
        test al, 0b10000000
        jz zero
    notzero:
        mov esi, [ebp - 8] # color
        mov dword ptr [edi], esi
    zero:
        shl al, 1
        add edi, 4
        loop putline

    add edi, (800 - 8)*4
    inc ebx
    pop ecx
    loop putlet

    mov esp, ebp
    pop ebp
    ret

// void PutStr(int (eax) x, (ebx) y, (ecx) color, char* (edx) str)
PutStr:
    push edx
    push ecx
    push ebx
    push eax
    # x=[esp] y=[esp+4] color=[esp+8] str=[esp+12]

1:  mov edx, [esp + 12] # str
    movzx edx, byte ptr [edx]
    cmp edx, 0
    jz 2f
    mov ecx, [esp + 8]
    mov ebx, [esp + 4]
    mov eax, [esp]
    call PutChar
    add dword ptr [esp], 8*4
    inc dword ptr [esp + 12]
    jmp 1b
2:  add esp, 16
    ret

endpic:
    # draw cross
    mov eax, [pframebuf]
    mov ebx, 0xffff00 # yellow
    mov dword ptr [eax + (800*300+400)*4], 0xff00 # green
    mov [eax + (800*300+400)*4 + 4], ebx
    mov [eax + (800*300+400)*4 + 4*2], ebx
    mov [eax + (800*300+400)*4 + 4*3], ebx
    mov [eax + (800*300+400)*4 + 4*4], ebx
    mov [eax + (800*300+400)*4 - 4], ebx
    mov [eax + (800*300+400)*4 - 4*2], ebx
    mov [eax + (800*300+400)*4 - 4*3], ebx
    mov [eax + (800*300+400)*4 - 4*4], ebx
    mov [eax + (800*300+400)*4 + 800*4], ebx
    mov [eax + (800*300+400)*4 + 800*4*2], ebx
    mov [eax + (800*300+400)*4 + 800*4*3], ebx
    mov [eax + (800*300+400)*4 + 800*4*4], ebx
    mov [eax + (800*300+400)*4 - 800*4], ebx
    mov [eax + (800*300+400)*4 - 800*4*2], ebx
    mov [eax + (800*300+400)*4 - 800*4*3], ebx
    mov [eax + (800*300+400)*4 - 800*4*4], ebx

    # done
    ret

        .text
asm_exit:
        {.att_syntax noprefix | }
    )" ::: "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");
}
