.intel_syntax noprefix
.section data1, "awx"

object.type = 0

type.star = 1
star.type = 0
star.x = 4
star.y = 8
star.vx = 12
star.vy = 16

type.circle = 2
circle.type = 0
circle.x = 4
circle.y = 8
circle.vx = 12
circle.vy = 16
circle.colour = 20
circle.radius = 24
circle_size = 28

type.str = 3
str.type = 0
str.x = 4
str.y = 8
str.vx = 12
str.vy = 16
str.color = 20
str.string = 24
str.len = 28
str_size = 32

#.=0040d000
dt: .int .
    .align 16

mouse.x:
    .int 0
mouse.y:
    .int 0
pause:
    .int 0

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
    .global asm_main
asm_main:
    call srand

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
    mov edi, offset dot + 4
    mov ecx, 8
1:  # 0x12345678
    rol edx, 4
    # 0x23456781
    mov eax, edx
    and eax, 0x0f
    # 1 (00..0f)
    mov al, [hex + eax]
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
    cmp ebx, 0x1d0              # MouseMove
    jb  1f
    cmp ebx, 0x1ef              # Mouse....
    ja  1f

    mov ecx, eax	# extract x
    and ecx, 0b00000000000011111111111000000000
    shr ecx, 9
    mov edx, 799	# clamp x to 0..799
    cmp ecx, edx
    cmova ecx, edx
    mov [mouse.x], ecx

    mov ecx, eax	# extract y
    and ecx, 0b01111111111100000000000000000000
    shr ecx, 20
    mov edx, 599    # clamp y to 0..599
    cmp ecx, edx
    cmova ecx, edx
    mov [mouse.y], ecx

1:  cmp ebx, ' '   # Space
    jne 1f

    // Space
    not dword ptr [pause]
    jmp next

1:  cmp ebx, 0x1d1 # MouseLeft
    jne 1f

    // MouseLeft
    call create_circle
    jmp next

1:  cmp ebx, 0x1d3 # MouseRight
    jne 1f

    // MouseRight
    call create_string
    jmp next

1:  cmp ebx, 0x1d0 # MouseMove
    jne 1f

    // MouseMove
    jmp next

1:  cmp ebx, 0x1ff # NextFrame
    jne 1f

    // NextFrame
    cmp dword ptr [pause], 0
    jnz 2f
    call animate
2:  // skip animate() if paused

1:  // nothing of the above
    jmp next

//
// ALLOC
//

Heap:
    .space 2048 * 1024, 0
Heap_end:

Heap_pos:
    .int Heap

malloc:
    # eax - byte size for space
    push ebx
    mov ebx, [Heap_pos]
    add ebx, eax
    cmp ebx, offset Heap_end
    jg 1f
    mov eax, [Heap_pos]
    mov [Heap_pos], ebx
    jmp 2f
1:  xor eax, eax
2:  pop ebx
    ret

free:
    ret

//
// LCG PRNG - Linear Congruential Generator as Pseudo-Random Number Generator
//

    .align 8
seed:
    .int 1

// void srand(void)

srand:
    rdtsc
    mov [seed], eax
    ret

// int rand(void)

rand:
    mov eax, 973656667
    mul dword ptr [seed] # edx:eax
    add eax, 223939
    shrd eax, edx, 9
    mov [seed], eax
    ret

// int rand_good(void)

rand_good:
    mov eax, 0x01010101
    mul dword ptr [seed] # edx:eax
    add eax, 0x31415926
    shrd eax, edx, 6
    mov [seed], eax
    ret

//
// void animate(void)
//

animate:
    # animate_star(&star1); // is equivalent of animate_s1();
    mov eax, offset star1
    call animate_star

    # animate_star(&star2);
    mov eax, offset star2
    call animate_star

/*
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

    # fill screen with random dots
    mov ecx, 300	# pixels per frame
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
*/

    mov eax, offset Heap
1:
    mov ebx, [eax + object.type]
    cmp ebx, 0
    jz 4f

    cmp ebx, type.circle
    jne 2f

    // type.circle
    push eax
    push ebx
    call animate_circle
    pop ebx
    pop eax
    add eax, circle_size
    jmp 1b

2:  cmp ebx, type.str
    jne 3f

    // type.str
    push eax
    push ebx
    call animate_str
    pop ebx
    pop eax
    add eax, str_size
    jmp 1b

3:  // type.???
4:  // zero type

    mov eax, 640
    mov ebx, 580
    mov ecx, 0xff3388
    mov edx, offset dot
    call PutStr

    ret

//
// STAR
//

    .align 8
star1:
    /*type*/ .int type.star
    /*  x */ .int 0
    /*  y */ .int 500
    /* vx */ .int 8
    /* vy */ .int 6

    .align 8
star2:
    /*type*/ .int type.star
    /*  x */ .int 100
    /*  y */ .int 100
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

    .global _PutStarC
    .global PutStarC
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

// void PutStar(int (eax) x, (ebx) y)

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

    .global _Clear32x32C
    .global Clear32x32C
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

// void Clear32x32(int (eax) x, (ebx) y, (ecx) color)

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


//
// CIRCLE
//

    .align 8
Circle1:
    /*type*/     .int type.circle
    /* x */      .int 90
    /* y */      .int 400
    /* vx */     .int 8
    /* vy */     .int -20
    /* colour */ .int 0x29fdbb
    /* radius */ .int 76

    .align 8
Circle2:
    /*type*/     .int type.circle
    /* x */      .int 140
    /* y */      .int 340
    /* vx */     .int 3
    /* vy */     .int 3
    /* colour */ .int 0xff2288
    /* radius */ .int 38

// void create_circle(key eax)

create_circle:

    mov eax, circle_size
    call malloc
    mov esi, eax
    cmp esi, 0
    jz 1f

    mov eax, [mouse.x]
    mov ebx, [mouse.y]
    mov dword ptr [esi + circle.type], type.circle

    mov [esi + circle.x], eax               #x
    mov [esi + circle.y], ebx               #y

    call rand                               #colour
    mov [esi + circle.colour], eax          #|

    call rand                               #radius
    xor edx, edx                            #|
    mov ebx, 60                             #|
    div ebx                                 #|
    add edx, 10                             #|
    mov [esi + circle.radius], edx          #|

    call rand                               #vx
    xor edx, edx                            #|
    mov ebx, 60                             #|
    div ebx                                 #|
    sub edx, 30                             #|
    mov [esi + circle.vx], edx              #|

    call rand                               #vy
    xor edx, edx                            #|
    mov ebx, 60                             #|
    div ebx                                 #|
    sub edx, 30                             #|
    mov [esi + circle.vy], edx              #|

1:  ret

// void animate_circle(circle *eax)

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
    mov edx, [esi + circle.radius]
    cmp eax, edx
    jl 1f
    mov ecx, 800
    sub ecx, edx
    cmp eax, ecx
    jg 1f
    cmp ebx, edx
    jl 1f
    mov ecx, 600
    sub ecx, edx
    cmp ebx, ecx
    jg 1f
    pusha
    xor ecx, ecx
    call PutCircle
    popa
1:
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

// void PutCircle(int (eax) xc, (ebx) yc, (ecx) colour, (edx) radius)

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


//
// STRING
//

letter.width = 8
letter.height = 13

strings_max = 100

    .align 8
n_strings:
    .int 0

strings:
    .space str_size * strings_max, 0

aiur:  .string "My life for Aiur!"

    .align 8
str1:
    /*type*/ .int type.str
    /*x  */  .int 130
    /*y  */  .int 130
    /*vx */  .int 4
    /*vy */  .int 6
    /*col*/  .int 0xff55aaff
    /*str*/  .int aiur
    /*len*/  .int 17

hello: .string "Hello there!"

    .align 8
str2:
    /*type*/ .int type.str
    /*x  */  .int 555
    /*y  */  .int 444
    /*vx */  .int 2
    /*vy */  .int 2
    /*col*/  .int 0xff00ff00
    /*str*/  .int hello
    /*len*/  .int 12

create_string:

    mov eax, str_size
    call malloc
    mov esi, eax
    cmp esi, 0
    jz 3f

    mov eax, [mouse.x]
    mov ebx, [mouse.y]
    mov dword ptr [esi + str.type], type.str

    mov [esi + str.x], eax                              #x
    mov [esi + str.y], ebx                              #y

    call rand                                           #colour
    mov [esi + str.color], eax                          #|

    call rand                                           #string
    xor edx, edx                                        #|
    mov ebx, 2                                          #|
    div ebx                                             #|
    cmp edx, 0                                          #|
    jz 1f                                               #|
    mov dword ptr [esi + str.string], offset aiur       #|
    mov dword ptr [esi + str.len], 17                   #|
    jmp 2f                                              #|
1:  mov dword ptr [esi + str.string], offset hello      #|
    mov dword ptr [esi + str.len], 12                   #|
2:
    call rand                                           #vx
    xor edx, edx                                        #|
    mov ebx, 20                                         #|
    div ebx                                             #|
    sub edx, 10                                         #|
    mov [esi + str.vx], edx                             #|

    call rand                                           #vy
    xor edx, edx                                        #|
    mov ebx, 20                                         #|
    div ebx                                             #|
    sub edx, 10                                         #|
    mov [esi + str.vy], edx                             #|

3:
    ret

// void animate_str(str *eax)

animate_str:
    mov esi, eax
    mov eax, [esi + str.x]
    mov ebx, [esi + str.y]
    mov edx, [esi + str.len]
    imul edx, edx, letter.width
    mov ecx, 800
    sub ecx, edx
    cmp eax, ecx
    jg 1f
    mov ecx, 600 - letter.height
    cmp ebx, ecx
    jg 1f
    mov edx, [esi + str.string]
    pusha
    xor ecx, ecx
    call PutStr
    popa
1:
    mov edx, [esi + str.string]
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
    add ebx, letter.height
    cmp ebx, 600
    jl 1f
    neg dword ptr [esi + str.vy]
    mov ebx, 600
1:  sub ebx, letter.height
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
