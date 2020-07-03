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
        .section data1, "awx"

#.=0040d000
dt: .int .
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

dot:   .string "dot=xxxxxxxx"
hex:   .string "0123456789abcdef"
s1:    .string "01234567890123456789012345678901234567890123456789012345678901234567890123456789"

    .align 8
asm_main:
/*
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

    mov eax, 100
    mov ebx, 140
    mov ecx, 0xff3388
    mov edx, 'A'
    call PutChar
*/

    #reserved = 00000000 = 0
    #code     = 00400000 = 4 Mb
    #asm_exit = 004015cd
    #main     = 00404d70
    #_start   = 0040c50c
    #dt       = 0040d000
    #asm_main = 0040e230
    #asm_end  = 0040e4df = 4 Mb 153 kb

    #fb_start = 02b00000 = 44 Mb
    #fb_end   =          = 45 Mb

    #________ =          = 45 Mb
    # ^  ^  ^
    # |  |  |
    #esp      = 02d0ff50 = 46 Mb 143 kb

    mov edi, offset s1
    call str_len

    mov edx, eax
    mov edi, offset dot+4
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
    mov edx, offset dot
    call PutStr

/*
    mov eax, 300
    mov ebx, 340
    mov ecx, 0xffffff
    call PutPix
    mov eax, 302
    mov ebx, 340
    mov ecx, 0xffffff
    call PutPix
    mov eax, 300
    mov ebx, 342
    mov ecx, 0xffffff
    call PutPix
    mov eax, 302
    mov ebx, 342
    mov ecx, 0xffffff
    call PutPix

    mov eax, 100
    mov ebx, 140
    mov ecx, 0xffffff
    mov edx, 60
    call PutCircle
    mov eax, 300
    mov ebx, 140
    mov ecx, 0xffffff
    mov edx, 60
    call PutCircle
    mov eax, 100
    mov ebx, 340
    mov ecx, 0xffffff
    mov edx, 60
    call PutCircle
    mov eax, 300
    mov ebx, 340
    mov ecx, 0xffffff
    mov edx, 60
    call PutCircle

    mov eax, 600
    mov ebx, 540
    mov ecx, 0xff3388
    mov edx, 'B'
    call PutChar
*/

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

    mov ebx, eax	# extract x
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

    #call animate

    mov ecx, 300	# pix per frame
1:  push ecx
    call rand
    xor edx, edx
    mov ecx, 600
    div ecx
    mov ebx, edx	# y
    call rand
    xor edx, edx
    mov ecx, 800
    div ecx
    mov eax, edx	# x
    mov ecx, 0x1EAE98	# color
    call PutPix
    pop ecx
    loop 1b

    jmp next
    jmp endpic

    .align 8
seed:
    .int 1

rand:
    mov eax, 973656667
    mul dword ptr [seed] # edx:eax
    add eax, 223939
    shrd eax, edx, 9
    mov [seed], eax
    ret

rand_good:
    mov eax, 0x01010101
    mul dword ptr [seed] # edx:eax
    add eax, 0x31415926
    shrd eax, edx, 6
    mov [seed], eax
    ret

animate:
    # animate_star(&star1); // is equivalent of animate_s1();
    mov eax, offset star1
    call animate_star

    # animate_star(&star2);
    mov eax, offset star2
    call animate_star

    mov eax, offset Circle1
    call animate_circle

    mov eax, offset Circle2
    call animate_circle

    # animate_str(&str1);
    mov eax, offset str1
    call animate_str

    # animate_str(&str2);
    mov eax, offset str2
    call animate_str

    mov eax, 640
    mov ebx, 580
    mov ecx, 0xff3388
    mov edx, offset dot
    call PutStr

    ret

star.x = 0
star.y = 4
star.vx = 8
star.vy = 12

    .align 8
star1:
    /* x */  .int 0
    /* y */  .int 500
    /* vx */ .int 8
    /* vy */ .int 6

    .align 8
star2:
    /* x */  .int 100
    /* y */  .int 100
    /* vx */ .int 1
    /* vy */ .int 1

// function animate_star(star *s (eax));

animate_star:
    mov esi, eax
    mov eax, [esi + star.x]
    mov ebx, [esi + star.y]
    xor ecx, ecx
    call Clear32x32

    mov eax, [esi + star.x]
    add eax, [esi + star.vx]
    cmp eax, 800 - 32
    jl 1f
    neg dword ptr [esi + star.vx]
    mov eax, 800 - 32
1:  cmp eax, 0
    jg 1f
    neg dword ptr [esi + star.vx]
    mov eax, 0
1:  mov [esi + star.x], eax

    mov ebx, [esi + star.y]
    add ebx, [esi + star.vy]
    cmp ebx, 600 - 32
    jl 1f
    neg dword ptr [esi + star.vy]
    mov ebx, 600 - 32
1:  cmp ebx, 0
    jg 1f
    neg dword ptr [esi + star.vy]
    mov ebx, 0
1:  mov [esi + star.y], ebx

    call PutStar
    ret

// void PutStarC(int x, y)
// void PutStar(int (eax) x, (ebx) y)

    .global _PutStarC
_PutStarC:
PutStarC:
    push ebx
    push esi
    push edi
    mov eax, [esp + 16]
    mov ebx, [esp + 20]
    call PutStar
    pop edi
    pop esi
    pop ebx
    ret
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

// void Clear32x32C(int x, y, color)
// void Clear32x32(int (eax) x, (ebx) y, (ecx) color)

    .global _Clear32x32C
_Clear32x32C:
Clear32x32C:
    push ebx
    push esi
    push edi
    mov eax, [esp + 16]
    mov ebx, [esp + 20]
    mov ecx, [esp + 24]
    call Clear32x32
    pop edi
    pop esi
    pop ebx
    ret
Clear32x32:
    imul edi, ebx, 800
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov eax, ecx
    mov ecx, 32
1:  mov ebx, ecx
    mov ecx, 32
    rep stosd
    add edi, (800 - 32) * 4
    mov ecx, ebx
    loop 1b
    ret

// void PutCircle(int (eax) xc, (ebx) yc, (ecx) colour, (edx) radius)

circle.x = 0
circle.y = 4
circle.vx = 8
circle.vy = 12
circle.colour = 16
circle.radius = 20

    .align 8
Circle1:
    /* x */      .int 90
    /* y */      .int 400
    /* vx */     .int 8
    /* vy */     .int -20
    /* colour */ .int 0x29fdbb
    /* radius */ .int 76

    .align 8
Circle2:
    /* x */      .int 140
    /* y */      .int 340
    /* vx */     .int 3
    /* vy */     .int 3
    /* colour */ .int 0xff2288
    /* radius */ .int 38

animate_circle:
    mov esi, eax

    mov eax, [esi + circle.vy]
    inc eax
    mov ebx, 4000
    cmp eax, ebx
    cmovg eax, ebx
    mov ebx, -4000
    cmp eax, ebx
    cmovl eax, ebx
    mov [esi + circle.vy], eax

    mov eax, [esi + circle.x]
    mov ebx, [esi + circle.y]
    xor ecx, ecx
    mov edx, [esi + circle.radius]
    pusha
    call PutCircle
    popa

    mov eax, [esi + circle.x]
    add eax, [esi + circle.vx]
    mov edi, 800
    sub edi, [esi + circle.radius]
    cmp eax, edi
    jl 1f
    neg dword ptr [esi + circle.vx]
    mov eax, edi
1:  cmp eax, [esi + circle.radius]
    jg 1f
    neg dword ptr [esi + circle.vx]
    mov eax, [esi + circle.radius]
1:  mov [esi + circle.x], eax

    mov ebx, [esi + circle.y]
    add ebx, [esi + circle.vy]
    mov edi, 600
    sub edi, [esi + circle.radius]
    cmp ebx, edi
    jl 1f
    neg dword ptr [esi + circle.vy]
    mov ebx, edi
1:  cmp ebx, [esi + circle.radius]
    jg 1f
    neg dword ptr [esi + circle.vy]
    mov ebx, [esi + circle.radius]
1:  mov [esi + circle.y], ebx

    mov ecx, [esi + circle.colour]
    call PutCircle
    ret

    .global _PutCircleC
_PutCircleC:
PutCircleC:
    push ebx
    push esi
    push edi
    mov eax, [esp + 16]
    mov ebx, [esp + 20]
    mov ecx, [esp + 24]
    mov edx, [esp + 28]
    call PutCircle
    pop edi
    pop esi
    pop ebx
    ret
PutCircle:
    push ebp
    mov ebp, esp

    push edx
    push ecx
    push ebx
    push eax
    sub esp, 8

    ebp.ptop = -24; ebp.pbot = -20
    ebp.xc = -16; ebp.yc = -12
    ebp.colour = -8; ebp.radius = -4

    /*
    edx = r*r;
    ptop = pbot = &pframebuf[yc][xc];
    for (yi = 0; yi < r; yi++):		// ebx
           xi = 0;			// eax
           ecx = yi*yi;
           do xi++ while (xi < r && xi*xi + ecx < edx);
           ecx = xi * 2 - 1;
           PutLine(ptop - xi*4, ecx, colour);
           PutLine(pbot - xi*4, ecx, colour);
       ptop -= 800*4;
       pbot += 800*4;
    */

    imul edx, edx		# edx = r^2
    imul edi, ebx, 800		# edi = &pframebuf[yc][xc]
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov [ebp + ebp.ptop], edi	# ptop = edi
    mov [ebp + ebp.pbot], edi	# pbot = edi

    xor ebx, ebx
1:  xor eax, eax		# xi = 0
    mov ecx, ebx
    imul ecx, ebx		# ecx = yi^2
    mov edi, [ebp + ebp.radius]	# edi = r
2:  inc eax			# xi++
    cmp eax, edi
    jge 3f
    mov esi, eax		# eax: xi, ebx: yi, ecx:yi^2, edx:r^2, esi:-, edi:-
    imul esi, eax		# esi = xi^2
    add esi, ecx		# esi = xi^2 + yi^2
    cmp esi, edx
    jl 2b
3:  mov esi, eax
    shl esi, 1			# esi = xi * 2

    # @now: eax:xi*4, ebx:yi, ecx:-, edx:r^2, esi:xi*2, edi:-
    # @stosd: eax:COLOUR, ebx:yi, ecx:xi*2-1, edx:r^2, esi:SAVECX, edi:PTR
    mov eax, [ebp + ebp.colour]

    lea edi, [esi * 2]		# edi = xi*4
    neg edi			# edi = -xi*4
    add edi, [ebp + ebp.ptop]	# edi = ptop - xi*4
    lea ecx, [esi - 1]		# ecx = xi * 2 - 1
    rep stosd es:[edi], eax	# PutLine(ptop - xi*4, ecx, colour)

    lea edi, [esi * 2]		# edi = xi*4
    neg edi			# edi = -xi*4
    add edi, [ebp + ebp.pbot]	# edi = ptop - xi*4
    lea ecx, [esi - 1]		# ecx = xi * 2 - 1
    rep stosd es:[edi], eax	# PutLine(pbot - xi*4, ecx, colour)

    sub dword ptr [ebp + ebp.ptop], 800*4	# ptop -= 800*4
    add dword ptr [ebp + ebp.pbot], 800*4	# pbot += 800*4

    inc ebx
    cmp ebx, [ebp + ebp.radius]
    jl 1b

    mov esp, ebp
    pop ebp
    ret

    .global _PutCircleC2
_PutCircleC2:
PutCircleC2:
    push ebx
    push esi
    push edi
    mov eax, [esp + 16]
    mov ebx, [esp + 20]
    mov ecx, [esp + 24]
    mov edx, [esp + 28]
    call PutCircle2
    pop edi
    pop esi
    pop ebx
    ret
PutCircle2:
    push ebp
    mov ebp, esp

    push edx
    push ecx
    push ebx
    push eax
    sub esp, 8

    ebp.ptop = -24; ebp.pbot = -20
    ebp.xc = -16; ebp.yc = -12
    ebp.colour = -8; ebp.radius = -4

    /*
    edx = r*r;
    ptop = pbot = &pframebuf[yc][xc];
    for (yi = 0; yi < r; yi++):		// ebx
           xi = 0;			// eax
           ecx = yi*yi;
           do xi++ while (xi < r && xi*xi + ecx < edx);
           ecx = xi * 2 - 1;
           PutLine(ptop - xi*4, ecx, colour);
           PutLine(pbot - xi*4, ecx, colour);
       ptop -= 800*4;
       pbot += 800*4;
    */

    imul edx, edx		# edx = r^2
    imul edi, ebx, 800		# edi = &pframebuf[yc][xc]
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov [ebp + ebp.ptop], edi	# ptop = edi
    mov [ebp + ebp.pbot], edi	# pbot = edi

    xor ebx, ebx
1:  xor eax, eax		# xi = 0
    mov ecx, ebx
    imul ecx, ebx		# ecx = yi^2
    mov edi, [ebp + ebp.radius]	# edi = r
2:  inc eax			# xi++
    cmp eax, edi
    jge 3f
    mov esi, eax		# eax: xi, ebx: yi, ecx:yi^2, edx:r^2, esi:-, edi:-
    imul esi, eax		# esi = xi^2
    add esi, ecx		# esi = xi^2 + yi^2
    cmp esi, edx
    jl 2b
3:  shl eax, 1			# eax = xi * 2
    mov ecx, eax		# ecx = xi * 2
    dec ecx			# ecx = xi * 2 - 1
    shl eax, 1			# eax = xi * 4

    # @now: eax:xi*4, ebx:yi, ecx:xi*2-1, edx:r^2, esi:-, edi:-
    # @stosd: stack:xi*4, eax:COLOUR, ebx:yi, ecx:xi*2-1, edx:r^2, esi:SAVECX, edi:PTR

    mov esi, ecx		# save ecx
    push eax
    mov edi, [ebp + ebp.ptop]
    sub edi, eax		# edi = ptop - xi*4
    mov eax, [ebp + ebp.colour]
    rep stosd es:[edi], eax	# PutLine(ptop - xi*4, ecx, colour)

    mov ecx, esi		# restore ecx
    pop esi
    mov edi, [ebp + ebp.pbot]
    sub edi, esi		# edi = pbot - xi*4
    rep stosd es:[edi], eax	# PutLine(pbot - xi*4, ecx, colour)

    sub dword ptr [ebp + ebp.ptop], 800*4	# ptop -= 800*4
    add dword ptr [ebp + ebp.pbot], 800*4	# pbot += 800*4

    inc ebx
    cmp ebx, [ebp + ebp.radius]
    jl 1b

    mov esp, ebp
    pop ebp
    ret

// void PutPix(int (eax) x, (ebx) y, (ecx) colour)

PutPix:
    imul edi, ebx, 800
    add edi, eax
    shl edi, 2
    add edi, [pframebuf]
    mov [edi], ecx
    ret

// void PutPic16x24(int (eax) x, (ebx) y)

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

// void ClearScreen()

ClearScreen:
    push eax
    xor eax, eax
    mov ecx, 800*600
    mov edi, [pframebuf]
    rep stosd
    pop eax
    ret

str.x = 0
str.y = 4
str.vx = 8
str.vy = 12
str.color = 16
str.string = 20
str.len = 24

    .align 8
str1:
    /* x*/  .int 130
    /* y*/  .int 130
    /*vx*/  .int 4
    /*vy*/  .int 6
    /*col*/ .int 0xff55aaff
    /*str*/ .int aiur
    /*len*/ .int 17

aiur:  .string "My life for Aiur!"

    .align 8
str2:
    /* x*/  .int 555
    /* y*/  .int 444
    /*vx*/  .int 2
    /*vy*/  .int 2
    /*col*/ .int 0xff00ff00
    /*str*/ .int hello
    /*len*/ .int 12

hello: .string "Hello there!"

animate_str:
    mov esi, eax
    mov eax, [esi + str.x]
    mov ebx, [esi + str.y]
    xor ecx, ecx
    mov edx, [esi + str.string]
    pusha
    call PutStr
    popa

    mov edi, [esi + str.len]
    shl edi, 3

    mov eax, [esi + str.x]
    add eax, [esi + str.vx]
    add eax, edi
    cmp eax, 800
    jl 1f
    neg dword ptr [esi + str.vx]
    mov eax, 800
1:  sub eax, edi
    cmp eax, 0
    jg 1f
    neg dword ptr [esi + str.vx]
    mov eax, 0
1:  mov [esi + str.x], eax

    mov ebx, [esi + str.y]
    add ebx, [esi + str.vy]
    add ebx, 13
    cmp ebx, 600
    jl 1f
    neg dword ptr [esi + str.vy]
    mov ebx, 600
1:  sub ebx, 13
    cmp ebx, 0
    jg 1f
    neg dword ptr [esi + str.vy]
    mov ebx, 0
1:  mov [esi + str.y], ebx

    mov ecx, [esi + str.color]
    call PutStr
    ret

// int str_len(char* (edi) str)

str_len:
    push ecx
    xor eax, eax
    xor ecx, ecx
    not ecx
    repnz scasb # cmp al, [edi]; dec ecx
    mov eax, ecx
    pop ecx
    not eax
    dec eax
    ret

// int str_len_count(char* (edx) str)

str_len_count:
    mov edi, edx
1:  mov al, byte ptr [edi]
    inc edi
    test al, al
    jnz 1b
    dec edi
    sub edi, edx
    mov dword ptr [esi + str.len], edi
    ret

// void PutChar(int (eax) x, (ebx) y, (ecx) color, (edx) ch)

PutChar:
    push ebp
    mov ebp, esp

    push edx
    push ecx
    push ebx
    push eax
    # x=[ebp-16] y=[ebp-12] color=[ebp-8] ch=[ebp-4]

    mov ebx, offset font8x13
    mov eax, [ebp - 4] # ch
    imul eax, eax, 13
    add ebx, eax                # ebx - font char data
    imul edi, [ebp - 12], 800 # y
    add edi, [ebp - 16] # x
    shl edi, 2
    add edi, [pframebuf]

    mov ecx, 13
    putlet:
        push ecx
        mov ecx, 8
        mov al, [ebx]

    putline:
        shl al, 1
        jnc zero
    notzero:
        mov esi, [ebp - 8] # color
        mov dword ptr [edi], esi
    zero:
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
    add dword ptr [esp], 8
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

// void strlen_testC(int repeat)

    .global _strlen_testC
_strlen_testC:
strlen_testC:
    push ebx
    push esi
    push edi
    mov ecx, [esp + 16]
1:  mov edi, offset s1
    call str_len
    cmp eax, 80
    jne 2f
    loop 1b
2:  pop edi
    pop esi
    pop ebx
    ret

    # . = 0x0040e4df
asm_end:

        .text
asm_exit:
        {.att_syntax noprefix | }
    )" ::: "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");
}


extern "C" {
    // import
    void Clear32x32C(int x, int y, int color);
    void PutStarC(int x, int y);
    void PutCircleC(int x, int y, int color, int radius);
    void PutCircleC2(int x, int y, int color, int radius);
    void strlen_testC(int repeat);
    // export
    void animate_starC(struct star *s);
    void animateC();
}

struct star {
    int x;
    int y;
    int vx;
    int vy;
};

void animate_starC(struct star *s)
{
    Clear32x32C(s->x, s->y, 9548987835*0);
    s->x = s->x + s->vx;
    if (s->x >= 800-32) {
        s->vx = -s->vx;
        s->x = 800-32;
    }
    if (s->x <= 0) {
        s->vx = -s->vx;
        s->x = 0;
    }
    s->y = s->y + s->vy;
    if (s->y >= 600-32) {
        s->vy = -s->vy;
        s->y = 600-32;
    }
    if (s->y <= 0) {
        s->vy = -s->vy;
        s->y = 0;
    }

    PutStarC(s->x, s->y);
}

star star1c = {200, 100, 8, 6};
star star2c = {300, 500, 1, 1};

void animateC()
{
    animate_starC(&star1c);
    animate_starC(&star2c);
}

#ifdef MAIN
// g++ -DMAIN -fno-pic -fno-pie -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti asm1.cpp && ./a.out
// i686-w64-mingw32-g++ -DMAIN -fno-pic -fno-pie -mconsole -static-libgcc -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti asm1.cpp && wine ./a.exe

#include <stdio.h>

typedef unsigned long long uint64_t;

framebuf_t framebuf;
framebuf_t *pframebuf = &framebuf;
int waitkey(void) { return 0; }
int getkey(void) { return 0; }
char font8x13[13*256];

#ifdef WIN32
asm volatile (R"(
    // export
    pframebuf = _pframebuf
    waitkey = _waitkey
    getkey = _getkey
    font8x13 = _font8x13
)");
#endif

uint64_t rdtsc (void)
{
    unsigned tickl, tickh;
    asm ("rdtsc" : "=a" (tickl), "=d" (tickh));
    return (uint64_t) tickh << 32 | tickl;
}

int main()
{
    uint64_t beg, end;

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        asm volatile ("");
    }
    end = rdtsc();
    uint64_t eloop = end - beg;
    printf("eloop = %.2f\n", 1.0/1000000 * eloop);
/*
    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        PutCircleC(100, 100, 0xff1133, 44);
    }
    end = rdtsc();
    uint64_t PutCircle1 = end - beg;
    printf("PutCircle1 = %.2f\n", 1.0/1000000 * PutCircle1);

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        PutCircleC2(100, 100, 0xff1133, 44);
    }
    end = rdtsc();
    uint64_t PutCircle2 = end - beg;
    printf("PutCircle2 = %.2f\n", 1.0/1000000 * PutCircle2);
*/
    beg = rdtsc();
    for (int i = 0; i < 100; i++) {
        strlen_testC(100000);
    }
    end = rdtsc();
    uint64_t strlen_test1 = end - beg;
    printf("strlen_test = %.2f\n", 1.0/1000000 * strlen_test1);
}
#endif
