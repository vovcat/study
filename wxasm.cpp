struct Key {
    enum KeyEnum : int {
        Nokey = 0, Space = 0x20, At = 0x40, A = 0x41, a = 0x61, z = 0x7a,
        Nil = 0x100, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
        ScrollLock = 0x110, Pause, Ins, Del, CapsLock,
        ShiftL = 0x120, ShiftR, ControlL, WinL, AltL, AltR, WinR, Menu, ControlR,
        Home = 0x130, Left, Up, Right, Down, PgUp, PgDn, End, FirstKey = F1, LastKey = 0x1df,
        MouseMove = 0x1e0, MouseLeft, MouseMiddle, MouseRight, MousePgUp, MousePgDn,
        MAX = 0x1ff, SIZE = 0x200
    };
};

extern "C" {
    const int FB_WIDTH = 800, FB_HEIGHT = 600, FB_STRIDE = FB_WIDTH * sizeof(unsigned);
    typedef unsigned int framebuf_t[FB_HEIGHT][FB_WIDTH];
    extern framebuf_t *pframebuf;
    int getkey(int wait);
}

void rotl(unsigned &x, int width, int n) {
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

void asm_main(void)
{
    unsigned color = 0x00000ff;
    draw_cbox(color);

    while (1) {
        int x = rand() % FB_WIDTH, y = rand() % FB_HEIGHT;
        if (x > 0 && y > 0) { pframebuf[0][y-1][x-1] = color; }
        if (         y > 0) { pframebuf[0][y-1][x] = color; }
        if (x > 0         ) { pframebuf[0][y][x-1] = color; }
                            { pframebuf[0][y][x] = color; }

        if (int key = getkey(0)) {
            if (key == 'p') {
                draw_box((char *)pframebuf);
            } else if (key == 'l') {
                draw_ruler((char *)pframebuf);
            } else if (key == 'o') {
                rotl(color, 24, 1);
                printf("key='%c' color=%#08x\n", key, color);
                draw_cbox(color);
            }
        }
    }
}
