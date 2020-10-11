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
    void srand();
    unsigned rand();
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

void PutPix(int x, int y, int color)
{
    pframebuf[0][y][x] = color;
}

void PutCircleNewC(int x, int y, int color, int radius)
{
    int ch_x = x - radius;
    int ch_y = y - radius;
    for (int i = ch_y; i < y + radius; ++i) {
        for (int j = ch_x; j < x + radius; ++j) {
            if (((j - x)*(j - x) + (i - y)*(i - y)) < (radius * radius)) {
                if (((j >= 0) && (j < 800)) && ((i >= 0) && (i < 600))) {
                        PutPix(j, i, color ? color + j + i : 0);
                }
            }
        }
    }
}

struct rad_grad_param
{
    int inner_color;
    int outer_color;
};

double sqrt(double);

int RadGradTex(int dx, int dy, int r, void *param)
{
    rad_grad_param *p = (rad_grad_param *) param;
    int inner_color = p->inner_color;
    int outer_color = p->outer_color;

    int L = dx*dx + dy*dy;
    // L=0 => ret inner_color
    // L=r*r => ret outer_color
    // L=r*r => ret outer_color
    double t = 1.0 * L / r / r;
    //return ((1.0 - t) * inner_color + t * outer_color);
    int m = 0xff;
    int r_in = (inner_color >> 16) & m;
    int g_in = (inner_color >>  8) & m;
    int b_in = (inner_color >>  0) & m;
    int r_out = (outer_color >> 16) & m;
    int g_out = (outer_color >>  8) & m;
    int b_out = (outer_color >>  0) & m;
    return (((int)((1.0-t) * r_in + t * r_out) << 16) +
            ((int)((1.0-t) * g_in + t * g_out) <<  8) +
            ((int)((1.0-t) * b_in + t * b_out) <<  0));
}

int RandTex(int /*dx*/, int /*dy*/, int /*r*/, void */*param*/)
{
    return rand();
}

int SolidBlackTex(int /*dx*/, int /*dy*/, int /*r*/, void */*param*/)
{
    return 0;
}

void PutTexturedCircle(int x, int y, int radius, int (*texture)(int dx, int dy, int r, void *param), void *param)
{
    int ch_x = x - radius;
    int ch_y = y - radius;

    for (int i = ch_y; i < y + radius; ++i) {
        for (int j = ch_x; j < x + radius; ++j) {
            if (((j - x)*(j - x) + (i - y)*(i - y)) < (radius * radius)) {
                if (((j >= 0) && (j < 800)) && ((i >= 0) && (i < 600))) {
                        PutPix(j, i, texture(j-x, i-y, radius, param));
                }
            }
        }
    }
}

const int YELLOW = 0xffff00, GREEN = 0x00ff00;

void test()
{
    PutTexturedCircle(100, 100, 10, RandTex, NULL);
    PutTexturedCircle(100, 100, 10, SolidBlackTex, NULL);
    rad_grad_param p1 = { YELLOW, GREEN };
    PutTexturedCircle(100, 100, 10, RadGradTex, &p1);
}


int pause = 1;
int mouse_x = 0;
int mouse_y = 0;

struct screen_obj
{
    screen_obj *next;
    int type;
    virtual bool testxy(int x, int y) = 0;
    virtual void clear() = 0;
    //virtual void move(int dx, int dy) = 0;
    virtual void animate() = 0;
};

//
// STAR
//

const int type_star = 1;
struct star : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;

    star(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy) { }

    virtual bool testxy(int cx, int cy)
    {
        if (((cx > x) && (cx < x + 32)) &&
            ((cy > y) && (cy < y + 32))) {
                return true;
            }
        else return false;
    }

    virtual void clear()
    {
        Clear32x32C(x, y, 9548987835*0);
    }

    virtual void animate()
    {
        clear();
        x += vx;
        if (x >= 800-32) {
            vx = -vx;
            x = 800-32;
        }
        if (x <= 0) {
            vx = -vx;
            x = 0;
        }
        y += vy;
        if (y >= 600-32) {
            vy = -vy;
            y = 600-32;
        }
        if (y <= 0) {
            vy = -vy;
            y = 0;
        }
        PutStarC(x, y);
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

    virtual bool testxy(int cx, int cy)
    {
        if (((cx - x) * (cx - x) + (cy - y) * (cy - y)) < (radius * radius)) {
            return true;
        }
        else return false;
    }

    virtual void clear()
    {
        if (x >= 800 - 1 - radius) x  = 800 - 1 - radius;
        if (x <= radius)       x  = radius;
        if (y >= 600 - 1 - radius) y  = 600 - 1 - radius;
        if (y <= radius)       y  = radius;
        PutCircleNewC(x, y, 0, radius);
    }

    virtual void animate()
    {
        if (rand()&1) vy++;
        else vy--;
        clear();
        x += vx;
        if (x >= 800 - 1 - radius) {
            x  = 800 - 1 - radius;
            vx = -vx;
        }
        if (x <= radius) {
            x  = radius;
            vx = -vx;
        }
        y += vy;
        if (y >= 600 - 1 - radius) {
            y  = 600 - 1 - radius;
            vy = -vy;
        }
        if (y <= radius) {
            y  = radius;
            vy = -vy;
        }
//        PutTexturedCircle(x, y, radius, SolidBlackTex, NULL);
        rad_grad_param p1 = { YELLOW, GREEN };
        PutTexturedCircle(x, y, radius, RadGradTex, &p1);
    }
};
circle* circle_createC();

const int type_str = 3;
const int let_w = 8;
const int let_h = 13;
struct str : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;
    int color;
    const char *str;
    int len;

    virtual bool testxy(int cx, int cy)
    {
        if (((cx > x) && (cx < x + len * let_w)) &&
            ((cy > y) && (cy < y + let_h))) {
                return true;
            }
        else return false;
    }

    virtual void clear()
    {
        if (x >= 800 - 1 - len * let_w) x  = 800 - 1 - len * let_w;
        if (x <= 0) x = 0;
        if (y >= 600 - 1 - let_h) y  = 600 - 1 - let_h;
        if (y <= 0) y = 0;
        PutStrC(x, y, 0, str);
    }

    virtual void animate()
    {
        clear();
        x += vx;
        if (x >= 800 - 1 - len * let_w) {
            x  = 800 - 1 - len * let_w;
            vx = -vx;
        }
        if (x <= 0) {
            x  = 0;
            vx = -vx;
        }
        y += vy;
        if (y >= 600 - 1 - let_h) {
            y  = 600 - 1 - let_h;
            vy = -vy;
        }
        if (y <= 0) {
            y  = 0;
            vy = -vy;
        }
        PutStrC(x, y, color, str);
    }
};
str* str_createC();

struct list
{
    screen_obj *next;

    list() { next = NULL; }
    list(str *n) { next = n; }

    void add(screen_obj *po)
    {
        if (!po) return;
        screen_obj *pl = next;
        if (!pl) {
            next = po;
        } else {
            while (pl->next)
                pl = pl->next;
            pl->next = po;
        }
        po->next = NULL;
    }

    void walk(void (*f)(screen_obj *p))
    {
        screen_obj *pl = next;
        while (pl) {
            f(pl);
            pl = pl->next;
        }
    }

    screen_obj *findlast(bool (*P)(screen_obj *p))
    {
        screen_obj *plast = 0;
        screen_obj *pl = next;
        while (pl) {
            if (P(pl)) plast = pl;
            pl = pl->next;
        }
        return plast;
    }

    static screen_obj *fl_helper_1(screen_obj *pl, bool (*P)(screen_obj *p))
    {
        if (!pl) return NULL;
        screen_obj *p = fl_helper_1(pl->next, P);
        if (p) return p;
        if (P(pl)) return pl;
        return NULL;
    }

    screen_obj *findlast_h1(bool (*P)(screen_obj *p))
    {
        return fl_helper_1(next, P);
    }

    screen_obj *findlast1(bool (*P)(screen_obj *p), screen_obj *pl = NULL)
    {
        if (!pl) {
            if (next) return findlast1(P, next);
            return NULL;
        }
        screen_obj *p = NULL;
        if (pl->next) p = findlast1(P, pl->next);
        if (p) return p;
        if (P(pl)) return pl;
        return NULL;
    }

    void insert(str *o)
    {
        o->next = this->next;
        this->next = o;
    }

    screen_obj *remove(screen_obj *o)
    {
        if (this->next == o) {
            this->next = o->next;
            return o;
        }
        screen_obj *p = this->next;
        while ((p->next != o) && (p->next != NULL)) {
            p = p->next;
        }
        if (p->next) {
            p->next = o->next;
            return o;
        }
        return NULL;
    }
};

list screen_list;

bool fn(screen_obj *){ return true; }

bool mouse_pos_testerC(screen_obj *object)
{
    return object->testxy(mouse_x, mouse_y);
}

void asm_main_text(void)
{
    srand();
    PutStrC(100, 100, 0xff1155, "Entaro Adun!");

    while (1) {
        int getkey_val = waitkey();
        int key = getkey_val;
        key = key & 0b111111111;
        if ((key >= Key::MouseMove) && (key <= Key::MouseRelX2)) {
            mouse_x = getkey_val;
            mouse_x = mouse_x & 0b00000000000011111111111000000000;
            mouse_x = mouse_x >> 9;
            if (mouse_x > 800 - 1) mouse_x = 800 - 1;
            mouse_y = getkey_val;
            mouse_y = mouse_y & 0b01111111111100000000000000000000;
            mouse_y = mouse_y >> 20;
            if (mouse_y > 600 - 1) mouse_y = 600 - 1;
        }
        // PAUSE
        if (key == ' ') {
            pause = !pause;
        }
        // MOUSE_LEFT
        else if (key == 'l') {
            printf("last = %p\n", screen_list.findlast1([](screen_obj *) -> bool { return 1; }));
        }
        // MOUSE_LEFT
        else if (key == Key::MouseLeft) {
            screen_list.add(circle_createC());
        }
        // MOUSE_RIGHT
        else if (key == Key::MouseRight) {
            screen_list.add(str_createC());
        }

        // MOUSE_MIDDLE
        else if (key == Key::MouseMiddle) {
            screen_obj *a = screen_list.findlast1(&mouse_pos_testerC);
            if (a) {
                screen_list.remove(a);
                a->clear();
            }

        }

        // NEXT_FRAME
        else if (key == Key::NextFrame) {
            if (pause) animateC(); //check pause
        }
        // NO_ELSE
    }
}

//
// CIRCLE
//

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

//
// STRING
//

str* str_createC()
{
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

star star1c(200, 100, 8, 6);
star star2c(300, 500, 1, 1);

void animate_obj(screen_obj *p)
{
    p->animate();
}

void animateC()
{
    screen_list.walk(animate_obj);
    star1c.animate();
    star2c.animate();
    PutPix(400, 300, 0xffff00);
    PutCircleNewC(100, 100, 0xff0000, 20);
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
