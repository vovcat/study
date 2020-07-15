#include <string.h>
#include <stdio.h>

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
    int rand();
    void asm_mainC();
    void Clear32x32C(int x, int y, int color);
    void PutStarC(int x, int y);
    void PutCircleC(int x, int y, int color, int radius);
    void PutStrC(int x, int y, int color, const char *str);
    void strlen_testC(int repeat);
    // export
    void animate_starC(struct star *s);
    void animateC();
}

int pause;
int mouse_x = 0;
int mouse_y = 0;

struct screen_obj
{
    int *next;
    int type;
    //virtual int testxy() = 0;
    //virtual void move(int dx, int dy) = 0;
    virtual void animate() = 0;
};

struct star : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;
    star(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy) {}
    virtual void animate()
    {

    }
};

const int type_circle = 2;
struct circle : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;
    int colour;
    int radius;
    circle() {}
    circle(int x, int y, int vx, int vy, int color, int r) : x(x), y(y), vx(vx), vy(vy), colour(color), radius(r) {}
    virtual void animate() { }
};

const int type_str = 3;
struct str : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;
    int color;
    char *str;
    int len;
    virtual void animate() { }
};

int *screen_list;

void asm_main_text(void)
{
    PutStrC(100, 100, 0xff1155, "Entaro Adun!");
    while (1) {
        int getkey_val = getkey();
        int key = getkey_val;
        key = key & 0b111111111;
        if ((key >= Key::MouseMove) && (key <= Key::MouseRelX2)) {
            mouse_x = getkey_val;
            mouse_x = mouse_x & 0b00000000000011111111111000000000;
            mouse_x = mouse_x >> 9;
            if (mouse_x > 799) mouse_x = 799;
            mouse_y = getkey_val;
            mouse_y = mouse_y & 0b01111111111100000000000000000000;
            mouse_y = mouse_y >> 20;
            if (mouse_y > 599) mouse_y = 599;
        }
        //PAUSE
        if (key == ' ') {
            pause = -pause - 1;
        }
        //MOUSE_LEFT
        else if (key == Key::MouseLeft) {
//            list_addC(&screen_list, circle_createC());
            printf("Nagetsi \n");
        }
        //MOUSE_RIGHT
        else if (key == Key::MouseRight) {
//            list_addC(&screen_list, string_createC());
        }
/*
        //MOUSE_MOVE
        else if (key == Key::MouseMove) {
            int a = list_findlastC(&screen_list, &mouse_pos_testerC);
            if (a) obj_chg_col_under_mouseC(a);
        }
*/
        //NEXT_FRAME
        else if (key == Key::NextFrame) {
            if (!pause) animateC(); //check pause
        }
        //NO_ELSE
    }
}

int mouse_pos_testerC(screen_obj *object)
{
    //return object->testxy(mouse_x, mouse_y);
}

void obj_chg_col_under_mouseC(screen_obj *object)
{

}

//
// STAR
//

//star star1c(200, 100, 8, 6);
//star star2c(300, 500, 1, 1);

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

circle* circle_createC()
{
    circle *pc = new circle();
    if (pc) {
        pc->type = type_circle;
        pc->x = mouse_x;
        pc->y = mouse_y;
        pc->vx = rand() % 60 - 30;
        pc->vy = rand() % 60 - 30;
        pc->colour = rand();
        pc->radius = rand() % 60 + 10;
    }
    return pc;

    //pc = (circle *)malloc(sizeof(circle));
    //(*pc).circle(111, 222, 10, 10, 0xff1133, 22);

    /*
    mov eax, circle_size
    call malloc
    mov [pc], eax
    push 22
    push 0xff1133
    push 10
    push 10
    push 222
    push 111
    push [pc] //&c
    call circle
    add esp, 28
    */
}

str* str_createC() {
    str *ps = new str();
    if (ps) {
        ps->type = type_str;
        ps->x = mouse_x;
        ps->y = mouse_y;
        ps->vx = rand() % 30 - 15;
        ps->vy = rand() % 30 - 15;
        ps->color = rand();
        int r = rand() & 1;
        if (r) ps->str = "Nagetsi";
        else ps->str = "Vareniki";
        ps->len = strlen(ps->str);
    }
    return ps;
}

//
// ANIMATE
//

void animateC()
{
//    animate_starC(&star1c);
//    animate_starC(&star2c);
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
