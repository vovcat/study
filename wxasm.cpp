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
        MAX = 0x1ff, SIZE = 0x200
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
    int getkey(int wait);
    void asm_main(void);
}

void rotl(unsigned &x, int width, int n)
{
    x = ((x << n) | (x >> (width - n))) & ((1u << width) - 1);
}

void draw_box(char *shmimg_data)
{
    static int cnt = 0;
    for (int y = 100+cnt; y < std::min(200+cnt, FB_HEIGHT); y++) {
        unsigned *p = (unsigned *) (shmimg_data + FB_STRIDE * y);
        for (int x = 100+cnt; x < std::min(200+cnt, FB_WIDTH); x++)
            p[x] = 0xff1133;
    }
    cnt++;
}

void draw_ruler(char *shmimg_data)
{
    // draw ruler
    unsigned color = 0xffffff;
    for (int x = 0; x < FB_WIDTH+1; x += 100) {
        unsigned *p = (unsigned *) (shmimg_data);
        if (x > 0) {
            p[x-1 + 0 * FB_STRIDE/4] = color;
            p[x-1 + 1 * FB_STRIDE/4] = color;
            p[x-1 + 2 * FB_STRIDE/4] = color;
        }
        if (x < FB_WIDTH) {
            p[x + 0 * FB_STRIDE/4] = color;
            p[x + 1 * FB_STRIDE/4] = color;
            p[x + 2 * FB_STRIDE/4] = color;
            p[x + 3 * FB_STRIDE/4] = color;
            p[x + 4 * FB_STRIDE/4] = color;
            p[x + 5 * FB_STRIDE/4] = color;
        }
        if (x < FB_WIDTH-1) {
            p[x+1 + 0 * FB_STRIDE/4] = color;
            p[x+1 + 1 * FB_STRIDE/4] = color;
            p[x+1 + 2 * FB_STRIDE/4] = color;
        }
    }
    for (int y = 0; y < FB_HEIGHT+1; y += 100) {
        unsigned *p = (unsigned *) (shmimg_data + FB_STRIDE * y);
        if (y > 0) {
            p[0 - FB_STRIDE/4] = color;
            p[1 - FB_STRIDE/4] = color;
            p[2 - FB_STRIDE/4] = color;
        }
        if (y < FB_HEIGHT) {
            p[0] = color;
            p[1] = color;
            p[2] = color;
            p[3] = color;
            p[4] = color;
            p[5] = color;
        }
        if (y < FB_HEIGHT-1) {
            p[0 + FB_STRIDE/4] = color;
            p[1 + FB_STRIDE/4] = color;
            p[2 + FB_STRIDE/4] = color;
        }
    }
}

void draw_cbox(unsigned color)
{
    int maxx = FB_WIDTH, maxy = FB_HEIGHT;
    for (int y = maxy - 20; y < maxy; y++) {
        for (int x = maxx - 20; x < maxx; x++) {
            pframebuf[0][y][x] = (x==maxx-20 || x==maxx-1 || y==maxy-20 || y==maxy-1) ? 0x00ff1133 : color; // 00 rr gg bb
        }
    }
}

void Xasm_main(void)
{
    unsigned color = 0x00000ff;
    int mousedown = 0;
    draw_cbox(color);

    while (1) {
        int x = rand() % FB_WIDTH, y = rand() % FB_HEIGHT;
        if (x > 0 && y > 0) { pframebuf[0][y-1][x-1] = color; }
        if (         y > 0) { pframebuf[0][y-1][x] = color; }
        if (x > 0         ) { pframebuf[0][y][x-1] = color; }
                            { pframebuf[0][y][x] = color; }

        if (int key = getkey(0)) {
            if ((key & 511) == 'p') {
                draw_box((char *)pframebuf);
            } else if ((key & 511) == 'l') {
                draw_ruler((char *)pframebuf);
            } else if ((key & 511) == 'o') {
                rotl(color, 24, 1);
                printf("key='%c' color=%#08x\n", key, color);
                draw_cbox(color);
            } else if ((key & 511) == Key::MouseLeft) {
                mousedown = 1;
            } else if ((key & 511) == Key::MouseRelLeft) {
                mousedown = 0;
            } else if ((key & 511) == Key::MouseMove && mousedown) {
                int x = (key >> 9) & 2047;
                int y = (key >> 20) & 2047;
                int color = 0xff1133;
                //printf("MouseMove %d,%d\n", x, y);
                if (x > 0 && y > 0) { pframebuf[0][y-1][x-1] = color; }
                if (         y > 0) { pframebuf[0][y-1][x] = color; }
                if (x > 0         ) { pframebuf[0][y][x-1] = color; }
                                    { pframebuf[0][y][x] = color; }
            }
        }
    }
}

void asm_main_text()
{
    asm volatile (R"(
        {.intel_syntax noprefix | }
        jmp asm_exit

        .section data1, "awx"
        .align 0x1000
asm_start:

        .align 16

t1:	.string	"string here\n"
t2:     .ascii	"ascii here\n"
t3:     .asciz	"asciz here\n"
t_:
        .align 16
b1:	.byte	123
b_:
        .align 2
w1:	.short	0x234
w2:     .word	0235
w3:     .hword	0b00010001
w4:     .value	237
w_:
        .align 4
i1:	.int	345
i2:	.long	345
i_:
        .align 8
        .zero	8

        .align 8
        .space	8

# void seed(void)
# inputs: none  (modifies PRN seed variable)
# clobbers: eax  returns: AX = next random number
seed:	.int	.
srand:
        xor	eax, [seed]
        xor	eax, [esp]
        xor	eax, [esp+4]
        xor	eax, [esp+8]
        xor	eax, [esp+12]
        xor	eax, [esp+16]
        xor	eax, [esp+20]
        xor	eax, [esp+24]
        xor	eax, [esp+28]
        mov	[seed], eax		# Update seed = return value
        ret

# int rand(void)
# returns eax = next random number
rand2:
        mov     eax, 0xadb4a92d		# LCG Multiplier
        mul     dword ptr [seed]	# edx:eax = LCG multiplier * seed
        add	eax, 0xa13fc965		# Add LCG increment value
        shrd	eax, edx, 19
        mov	[seed], eax		# Update seed = return value
        ret
rand:
        mov     eax, 0x1010101		# LCG Multiplier
        mul     dword ptr [seed]	# edx:eax = LCG multiplier * seed
        add	eax, 0x31415927		# Add LCG increment value
        shrd	eax, edx, 16
        mov	[seed], eax		# Update seed = return value
        ret

        .global	_asm_main
asm_main:
        push	ebp
        mov	ebp, esp
        sub	esp, 8

nextpix:
        call	rand
        xor	edx, edx
        mov	ebx, 800
        div	ebx			# eax = Quotient, edx = Remainder
        mov	esi, edx		# X

        call	rand
        xor	edx, edx
        mov	ebx, 600
        div	ebx			# eax = Quotient, edx = Remainder
        mov	ebx, edx		# Y

        imul	ebx, 800		# y * 800
        add	ebx, esi		# y * 800 + x

        mov	esi, pframebuf
        mov	eax, 0xff1133		# color
        mov	[esi+ebx*4], eax

        push	0
        call	getkey
        add	sp, 4			# pop
        jmp	nextpix

        mov	esp, ebp
        pop	ebp
        ret

        .text
asm_exit:
        {.att_syntax noprefix | }
)"
        : /*out*/ //[i] "+m" (i), [ip] "+p" (i), [j] "+m" (j)
        : /*in*/  //[i] "a" (&i), [j] "b" (&j)
        : /*clobber*/ "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory"
    );
}
