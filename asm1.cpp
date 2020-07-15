#include <stdio.h>  // printf()
//#include <stdlib.h> // malloc()
#include <string.h> // strlen()

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

extern "C" {
    // import
    void Clear32x32C(int x, int y, int color);
    void PutStarC(int x, int y);
    void PutCircleC(int x, int y, int color, int radius);
    void PutStrC(int x, int y, int color, const char *str);
    void strlen_testC(int repeat);
    void srand(void);
    unsigned int rand(void);
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

struct screen_obj {
    screen_obj *next;
    int type;
    //virtual bool testxy(int x, int y) = 0;
    virtual void move(int dx, int dy) = 0;
    virtual void animate() = 0;
};

struct circle : screen_obj {
    int x, y, vx, vy, colour, radius;
    circle() {
        (*this).x = 123;
        this->x = 123;
        x = 123;
    }
    circle(int x_) {
        x = x_ + 123;
    }
    virtual void animate();
    virtual void move(int dx, int dy);
};

struct str : screen_obj {
    int x, y, vx, vy, colour;
    char *s;
    int len;
};

struct list {
    screen_obj *first;
    void insert(screen_obj *p){
        p->next = first;
        first = p;
    }
    void walk(void (*f)(screen_obj *p))
    {
        screen_obj *p = first;
        while (p != NULL) {
            f(p);
            p = p->next;
        }
    }
};

circle *circle_create()
{
    circle c1(12);
    printf("hello %d %x %s %d uff\n", c1.x, 1234, "string-here", (int)"haha");

    //circle *pc = (circle *) malloc(sizeof(circle));
    //(*pc).circle(12); == pc->circle(12);

    circle *pc = new circle(12);
    printf("new pc x = %d\n", pc->x);

    pc->x = rand() % 800;
    pc->y = rand() % 600;
    pc->vx = rand() % 60 - 30;
    pc->vy = rand() % 60 - 30;
    pc->radius = rand() % 60 + 10;
    pc->colour = rand();

    pc->move(0, 0);
    return pc;
}

void circle::animate()
{
    PutCircleC(x, y, 0, radius);
    move(vx, vy);
    if (x + radius == 800 - 1) {
        vx = -vx;
    }
    if (x - radius == 0) {
        vx = -vx;
    }
    if (y + radius == 600 - 1) {
        vy = -vy;
    }
    if (y - radius == 0) {
        vy = -vy;
    }
}

void circle::move(int dx, int dy)
{
    x += dx; // == x = x + dx;
    if (x + radius > 800 - 1) {
        x = 800 - 1 - radius;
    }
    if (x - radius < 0) {
        x = radius;
    }

    y += dy; // == y = y + dy;
    if (y + radius > 600 - 1) {
        y = 600 - 1 - radius;
    }
    if (y - radius < 0) {
        y = radius;
    }
    PutCircleC(x, y, colour, radius);
}

//
// ASM_MAIN_TEXT
//

void animate(screen_obj *p)
{
    p->animate();
}

void asm_main_text(void)
{
    srand();
    circle *pc = circle_create();
    list lst;

    while (1) {
        int k = waitkey();
        int x = (k >> 9) & 0x7FF; // 111'1111'1111 = 0x7FF
        int y = (k >> 20) & 0x7FF; // 111'1111'1111 = 0x7FF
        int e = k & 0x1FF;

        switch (e) {
        case Key::MouseMove:
            // hnhmh
            break;
        case Key::MouseLeft:
            lst.insert(circle_create());
            break;
        case Key::MouseRight:
            //
            break;
        case Key::NextFrame:
            animateC();
            pc->animate();
            lst.walk(animate);
            break;
        }
    }
    PutStrC(100, 100, 0xff1155, "Entaro Adun!");
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
