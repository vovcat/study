.intel_syntax noprefix
.section data1, "awx"

NULL = 0

object.next = 0
object.type = 4
object.testxy = 8
object.move = 12
object.animate = 16
object.end = 20

type.star = 1
star.x = object.end
star.y = star.x + 4
star.vx = star.y + 4
star.vy = star.vx + 4

type.circle = 2
circle.x = object.end
circle.y = circle.x + 4
circle.vx = circle.y + 4
circle.vy = circle.vx + 4
circle.colour = circle.vy + 4
circle.radius = circle.colour + 4
circle_size = circle.radius + 4

type.str = 3
str.x = object.end
str.y = str.x + 4
str.vx = str.y + 4
str.vy = str.vx + 4
str.color = str.vy + 4
str.string = str.color + 4
str.len = str.string + 4
str_size = str.len + 4

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
    .incbin "star.bmp", 54, 32 * 32 * 3

dot:   .string "dot=xxxxxxxx"
hex:   .string "0123456789abcdef"
s1:    .string "01234567890123456789012345678901234567890123456789012345678901234567890123456789"

    .align 8
    .global asm_main
asm_main:
    call srand

    mov eax, 2
    mov ebx, 2
    mov ecx, 0xffffff
    mov edx, 1
    call PutCircle
    mov eax, 5
    mov ebx, 5
    mov ecx, 0xffffff
    mov edx, 2
    call PutCircle
    mov eax, 10
    mov ebx, 10
    mov ecx, 0xffffff
    mov edx, 3
    call PutCircle
    mov eax, 17
    mov ebx, 17
    mov ecx, 0xffffff
    mov edx, 4
    call PutCircle
    mov eax, 25
    mov ebx, 25
    mov ecx, 0xffffff
    mov edx, 5
    call PutCircle
    mov eax, 35
    mov ebx, 35
    mov ecx, 0xffffff
    mov edx, 6
    call PutCircle
    mov eax, 46
    mov ebx, 46
    mov ecx, 0xffffff
    mov edx, 7
    call PutCircle
    mov eax, 60
    mov ebx, 60
    mov ecx, 0xffffff
    mov edx, 8
    call PutCircle
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
    call waitkey
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
    call circle_create
    mov ebx, eax
    mov eax, offset screen_list
    call list_add
    jmp next

1:  cmp ebx, 0x1d3 # MouseRight
    jne 1f

    // MouseRight
    call string_create
    mov ebx, eax
    mov eax, offset screen_list
    call list_add
    jmp next

1:  cmp ebx, 0x1d0 # MouseMove
    jne 1f

    // MouseMove
    mov eax, offset screen_list
    mov ebx, offset mouse_pos_tester
    call list_findlast
    cmp eax, NULL
    jz 2f
    call obj_move_from_mouse
2:  jmp next

1:  cmp ebx, 0x1ff # NextFrame
    jne 1f

    // NextFrame
    cmp dword ptr [pause], 0
    jnz 2f
    call animate
2:  // skip animate() if paused

1:  // nothing of the above
    jmp next

mouse_pos_tester:
    mov ebx, [mouse.x]
    mov ecx, [mouse.y]
    call [eax + object.testxy]	# object->testxy(mouse.x, mouse.y)
    ret

obj_move_from_mouse:
    push eax
    push ebx
    push ecx
    // change color
    xchg eax, edx
    call rand
    xchg eax, edx
    mov [eax + str.color], edx
    // redraw
    mov ebx, 0
    mov ecx, 0
    call [eax + object.move]	# object->move(0, 0)
    pop ecx
    pop ebx
    pop eax
    #jmp 1b
    ret

//
// ALLOC
//

.section .bss
Heap:    .space 2048 * 1024, 0
Heap_end:
.section "data1"

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
// LIST
//

    .align 8
screen_list:
    /*list_ptr*/ .int 0

list_add:
1:  cmp dword ptr [eax], 0
    jz 2f
    mov eax, [eax]
    jmp 1b
2:  # eax -> next, ebx -> object
    mov [eax], ebx
    mov dword ptr [ebx], 0
    mov eax, ebx
    ret

list_walk:
1:  cmp dword ptr [eax], 0
    jz 2f
    mov eax, [eax]
    push eax
    push ebx
    call ebx
    pop ebx
    pop eax
    jmp 1b
2:  ret

// screen_obj *list_findlast(screen_obj * list (eax), bool (*P)(screen_obj *p) (ebx))
// eax - list, ebx - P(screen_obj *p)

list_findlast:
    mov eax, [eax]
    mov esi, eax
    cmp esi, NULL
    jz 1f
    push esi
    call list_findlast
    pop esi
    cmp eax, NULL
    jnz 1f
    mov eax, esi
    push esi
    push ebx
    call ebx
    pop ebx
    pop esi
    cmp eax, NULL
    jz 1f
    mov eax, esi
1:  ret


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
    push edx
    mov eax, 973656667
    mul dword ptr [seed] # edx:eax
    add eax, 223939
    shrd eax, edx, 9
    mov [seed], eax
    pop edx
    ret

// int rand_good(void)

rand_good:
    push edx
    mov eax, 0x01010101
    mul dword ptr [seed] # edx:eax
    add eax, 0x31415926
    shrd eax, edx, 6
    mov [seed], eax
    pop edx
    ret

//
// void animate(void)
//

animate:
    # animate_star(&star1); // is equivalent of animate_s1();
    mov eax, offset star1
    call star_animate

    # animate_star(&star2);
    mov eax, offset star2
    call star_animate

/*
    mov eax, offset Circle1
    call circle_animate

    mov eax, offset Circle2
    call circle_animate

    # animate_str(&str1);
    mov eax, offset str1
    call str_animate

    # animate_str(&str2);
    mov eax, offset str2
    call str_animate

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

    mov eax, offset screen_list
    mov ebx, offset animate_obj
    call list_walk

    mov eax, 640
    mov ebx, 580
    mov ecx, 0xff3388
    mov edx, offset dot
    call PutStr

    ret

//
// ANIMATE_OBJ
//

animate_obj:
    call [eax + object.animate]
    ret

//
// STAR
//

    .align 8
star1:
    /*next*/    .int 0
    /*type*/    .int type.star
    /*testxy*/  .int 0
    /*move*/    .int 0
    /*animate*/ .int 0
    /*  x */    .int 0
    /*  y */    .int 500
    /* vy */    .int 6
    /* vx */    .int 8

    .align 8
star2:
    /*next*/    .int 0
    /*type*/    .int type.star
    /*testxy*/  .int 0
    /*move*/    .int 0
    /*animate*/ .int 0
    /*  x */    .int 100
    /*  y */    .int 100
    /* vx */    .int 1
    /* vy */    .int 1

// function animate_star(star *s (eax));

star_animate:
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
    /* next */   .int 0
    /* type */   .int type.circle
    /* testxy */ .int 0
    /* move */   .int 0
    /*animate*/  .int 0
    /* x */      .int 90
    /* y */      .int 400
    /* vx */     .int 8
    /* vy */     .int -20
    /* colour */ .int 0x29fdbb
    /* radius */ .int 76

    .align 8
Circle2:
    /* next */   .int 0
    /* type */   .int type.circle
    /* testxy */ .int 0
    /* move */   .int 0
    /*animate*/  .int 0
    /* x */      .int 140
    /* y */      .int 340
    /* vx */     .int 3
    /* vy */     .int 3
    /* colour */ .int 0xff2288
    /* radius */ .int 38

// circle *create_circle(void)

circle_create:
    mov eax, circle_size
    call malloc
    mov esi, eax
    cmp esi, 0
    jz 1f

    mov eax, [mouse.x]
    mov ebx, [mouse.y]
    mov dword ptr [esi + object.type], type.circle
    mov dword ptr [esi + object.testxy], offset circle_testxy
    mov dword ptr [esi + object.move], offset circle_move
    mov dword ptr [esi + object.animate], offset circle_animate

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

    mov eax, esi
1:  ret

// void circle_animate(circle *eax)

circle_animate:
    mov esi, eax
    // add gravity (increment vy)
    mov eax, [esi + circle.vy]
    inc eax
    mov ebx, 4000
    cmp eax, ebx
    cmovg eax, ebx
    mov [esi + circle.vy], eax
    // sanitize coordinates before clearing
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
    push esi
    xor ecx, ecx	# black
    call PutCircle	# clear
    pop esi
1:  // add vx, check for bounds
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
    // add vy, check for bounds
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
    // draw new
    mov ecx, [esi + circle.colour]
    mov edx, [esi + circle.radius]
    call PutCircle
    ret

// int circle_testxy(circle *eax, int (ebx) x, int (ecx) y)

circle_testxy:
    sub ebx, [eax + circle.x]
    sub ecx, [eax + circle.y]
    imul ebx, ebx
    imul ecx, ecx
    add ebx, ecx
    mov ecx, [eax + circle.radius]
    imul ecx, ecx
    xor eax, eax
    cmp ebx, ecx
    setle al
    ret

// void circle_move(circle *eax, int (ebx) dx, int (ecx) dy)

circle_move:
    push ecx		# dy
    push ebx		# dx
    // dx = [esp], dy = [esp + 4]
    mov esi, eax
    mov edx, [esi + circle.radius]
    // sanitize coordinates before clearing
    mov eax, [esi + circle.x]
    cmp eax, edx
    jl 1f
    mov ecx, 800
    sub ecx, edx
    cmp eax, ecx
    jg 1f
    mov ebx, [esi + circle.y]
    cmp ebx, edx
    jl 1f
    mov ecx, 600
    sub ecx, edx
    cmp ebx, ecx
    jg 1f
    push esi
    xor ecx, ecx	# black
    call PutCircle	# clear
    pop esi
1:  mov edx, [esi + circle.radius]
    // add vx, check for bounds
    mov eax, [esi + circle.x]
    add eax, [esp]	# dx
    mov ecx, 800
    sub ecx, edx	# circle.radius
    cmp eax, ecx
    jle 1f
    mov eax, ecx
1:  cmp eax, edx	# circle.radius
    jge 1f
    mov eax, edx	# circle.radius
1:  mov [esi + circle.x], eax
    // add vy, check for bounds
    mov ebx, [esi + circle.y]
    add ebx, [esp + 4]	# dy
    mov ecx, 600
    sub ecx, edx	# circle.radius
    cmp ebx, ecx
    jle 1f
    mov ebx, ecx
1:  cmp ebx, edx	# circle.radius
    jge 1f
    mov ebx, edx	# circle.radius
1:  mov [esi + circle.y], ebx
    // draw new
    mov ecx, [esi + circle.colour]
    call PutCircle
    pop ebx
    pop ecx
    ret

// void PutCircle(int (eax) xc, (ebx) yc, (ecx) colour, (edx) radius)

    .global _PutCircleC
_PutCircleC:
PutCircleC:
    push ebx
    push esi
    push edi
    mov eax, [esp + 16] # xc
    mov ebx, [esp + 20] # yc
    mov ecx, [esp + 24] # colour
    mov edx, [esp + 28] # radius
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
    /* next */   .int 0
    /* type */   .int type.str
    /* testxy */ .int 0
    /* move */   .int 0
    /*animate*/  .int 0
    /* x */      .int 130
    /* y */      .int 130
    /* vx */     .int 4
    /* vy */     .int 6
    /* color */  .int 0xff55aaff
    /* string */ .int aiur
    /* len */    .int 17

hello: .string "Hello there!"

    .align 8
str2:
    /* next */   .int 0
    /* type */   .int type.str
    /* testxy */ .int 0
    /* move */   .int 0
    /*animate*/  .int 0
    /* x */      .int 555
    /* y */      .int 444
    /* vx */     .int 2
    /* vy */     .int 2
    /* color */  .int 0xff00ff00
    /* string */ .int hello
    /* len */    .int 12

// str *create_string(void)

string_create:
    mov eax, str_size
    call malloc
    mov esi, eax
    cmp esi, 0
    jz 3f

    mov eax, [mouse.x]
    mov ebx, [mouse.y]
    mov dword ptr [esi + object.type], type.str
    mov dword ptr [esi + object.testxy], offset str_testxy
    mov dword ptr [esi + object.move], offset str_move
    mov dword ptr [esi + object.animate], offset str_animate

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

    mov eax, esi
3:  ret

// void str_animate(str *eax)

str_animate:
    mov esi, eax
    // sanitize coordinates before clearing
    mov eax, [esi + str.x]
    mov ebx, [esi + str.y]
    mov edx, [esi + str.len]
    shl edx, 3		# imul edx, edx, letter.width
    mov ecx, 800
    sub ecx, edx
    cmp eax, ecx
    jg 1f
    mov ecx, 600 - letter.height
    cmp ebx, ecx
    jg 1f
    push esi
    xor ecx, ecx	# black
    mov edx, [esi + str.string]
    call PutStr		# clear
    pop esi
1:  // add vx, check for bounds
    mov edi, [esi + str.len]
    shl edi, 3		# imul edi, edi, letter.width
    mov eax, [esi + str.x]
    add eax, [esi + str.vx]
    add eax, edi
    cmp eax, 800
    jl 1f
    neg dword ptr [esi + str.vx]
    mov eax, 800
1:  sub eax, edi
    jg 1f
    neg dword ptr [esi + str.vx]
    xor eax, eax
1:  mov [esi + str.x], eax
    // add vy, check for bounds
    mov ebx, [esi + str.y]
    add ebx, [esi + str.vy]
    add ebx, letter.height
    cmp ebx, 600
    jl 1f
    neg dword ptr [esi + str.vy]
    mov ebx, 600
1:  sub ebx, letter.height
    jg 1f
    neg dword ptr [esi + str.vy]
    xor ebx, ebx
1:  mov [esi + str.y], ebx
    // draw new
    mov ecx, [esi + str.color]
    mov edx, [esi + str.string]
    call PutStr
    ret

// int str_testxy (str *eax, int (ebx) x, int (ecx) y)

str_testxy:
    cmp ebx, [eax + str.x]
    jl 1f
    imul edx, [eax + str.len], letter.width
    add edx, [eax + str.x]
    cmp ebx, edx
    jge 1f
    cmp ecx, [eax + str.y]
    jl 1f
    mov edx, [eax + str.y]
    add edx, letter.height
    cmp ecx, edx
    jge 1f
    mov eax, 1
    jmp 2f
1:  xor eax, eax
2:  ret

// void str_move (str *eax, int (ebx) dx, int (ecx) dy)

str_move:
    push ecx		# dy
    push ebx		# dx
    // dx = [esp], dy = [esp + 4]
    mov esi, eax
    // sanitize coordinates before clearing
    mov eax, [esi + str.x]
    mov edx, [esi + str.len]
    shl edx, 3		# imul edx, edx, letter.width
    mov ecx, 800
    sub ecx, edx
    cmp eax, ecx
    jg 1f
    mov ebx, [esi + str.y]
    mov ecx, 600 - letter.height
    cmp ebx, ecx
    jg 1f
    push esi
    xor ecx, ecx	# black
    mov edx, [esi + str.string]
    call PutStr		# clear
    pop esi
1:
    // add dx, check for bounds
    mov eax, [esi + str.x]
    add eax, [esp]	# dx
    mov ecx, [esi + str.len]
    shl ecx, 3		# imul ecx, ecx, letter.width
    add eax, ecx
    cmp eax, 800
    jl 1f
    mov eax, 800
1:  sub eax, ecx
    jg 1f
    xor eax, eax
1:  mov [esi + str.x], eax
    // add dy, check for bounds
    mov ebx, [esi + str.y]
    add ebx, [esp + 4]	# dy
    add ebx, letter.height
    cmp ebx, 600
    jl 1f
    mov ebx, 600
1:  sub ebx, letter.height
    jg 1f
    xor ebx, ebx
1:  mov [esi + str.y], ebx
    // draw new
    mov ecx, [esi + str.color]
    mov edx, [esi + str.string]
    call PutStr
    pop ebx
    pop ecx
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
