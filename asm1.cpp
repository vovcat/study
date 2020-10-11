#include <stdio.h>  // printf()
#include <string.h> // strlen()
#include <cmath> // std::sqrt()

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
        StateShift =       0x01,
        StateCapsLock =    0x02,
        StateControl =     0x04,
        StateAlt =         0x08,
        StateMod2 =        0x10,
        StateMod3 =        0x20,
        StateMod4 =        0x40,
        StateWin =         0x80,
        StateMouseLeft =   0x100,
        StateMouseMiddle = 0x200,
        StateMouseRight =  0x400,
        StateMousePgUp =   0x800,
        StateMousePgDn =   0x1000,
        StateLang1 =       0x2000,
        StateLang2 =       0x4000,
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
    void srand(unsigned);
    int rand(void);
    // import
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

int mix_colors(int inner_color, int outer_color, double t)
{
    int icr = inner_color >> 16 & 0xFF;
    int icg = inner_color >> 8 & 0xFF;
    int icb = inner_color & 0xFF;
    int ocr = outer_color >> 16 & 0xFF;
    int ocg = outer_color >> 8 & 0xFF;
    int ocb = outer_color & 0xFF;
    return
        ((int((1.0-t) * icr + t * ocr) & 0xFF) << 16) +
        ((int((1.0-t) * icg + t * ocg) & 0xFF) << 8) +
        (int((1.0-t) * icb + t * ocb) & 0xFF);

}

int RadGradTex(int dx, int dy, int r, void *param)
{
    rad_grad_param *p = (rad_grad_param *) param;
    int inner_color = p->inner_color;
    int outer_color = p->outer_color;
    int L = dx*dx + dy*dy;
    float t = 1.0 * std::sqrt(L) / r;
    return mix_colors(inner_color, outer_color, t);

}

int HorizGradTex(int dx, int dy, int r, void *param)
{
    rad_grad_param *p = (rad_grad_param *) param;
    int inner_color = p->inner_color;
    int outer_color = p->outer_color;
    int L = dx;
    float t = 1.0 * L / (r + r) + 0.5;
    return mix_colors(inner_color, outer_color, t);

}

int DiagGradTex(int dx, int dy, int r, void *param)
{
    rad_grad_param *p = (rad_grad_param *) param;
    int inner_color = p->inner_color;
    int outer_color = p->outer_color;
    int L = dx + dy;
    float t = 1.0 / 1.4142 * L / (r + r) + 0.5;
    return mix_colors(inner_color, outer_color, t);

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

//const int YELLOW = 0xFC00FF, GREEN = 0x00DBDE;
//const int YELLOW = 0xC850C0, GREEN = 0x4158D0;
const int YELLOW = 0xFFFF00, GREEN = 0x00FF00;

void test()
{
    PutTexturedCircle(100, 100, 10, RandTex, NULL);
    PutTexturedCircle(100, 100, 10, SolidBlackTex, NULL);
    rad_grad_param p1 = { YELLOW, GREEN };
    PutTexturedCircle(100, 100, 10, RadGradTex, &p1);
}

int pause = 1;

// varable declaration using anonymous struct
struct {
    int x, y;
} mouse;

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

    virtual bool testxy(int x, int y)
    {
        return x >= this->x && x < (this->x + 32) &&
               y >= this->y && y < (this->y + 32);
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

    virtual bool testxy(int x, int y)
    {
        return (x - this->x)*(x - this->x) + (y - this->y)*(y - this->y) <= radius*radius;
    }

    virtual void clear()
    {
        if (rand()&1) vy++;
        else vy--;
        if (x >= 800 - 1 - radius) x  = 800 - 1 - radius;
        if (x <= radius)       x  = radius;
        if (y >= 600 - 1 - radius) y  = 600 - 1 - radius;
        if (y <= radius)       y  = radius;
        PutCircleNewC(x, y, 0, radius);
    }

    virtual void animate()
    {
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
        rad_grad_param p1 = { GREEN, YELLOW };
        PutTexturedCircle(x, y, radius, RadGradTex, &p1);
    }
};
circle* circle_createC();

const int type_str = 3;
struct str : screen_obj
{
    const int let_w = 8;
    const int let_h = 13;
    int x;
    int y;
    int vx;
    int vy;
    int color;
    const char *str;
    int len;

    virtual bool testxy(int x, int y)
    {
        return x >= this->x && x < (this->x + len * let_w) &&
               y >= this->y && y < (this->y + let_h);
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

struct list {
    screen_obj *first;

    list()
    {
        first = NULL;
    }

    void insert(screen_obj *p)
    {
        p->next = first;
        first = p;
    }

    void add(screen_obj *o)
    {
        screen_obj *p = first;
        if (p == NULL) {
            first = o;
            o->next = NULL;
            return;
        }
        while (p->next != NULL)
            p = p->next;

        p->next = o;
        o->next = NULL;
    }

    void walk(void (*f)(screen_obj *p))
    {
        screen_obj *p = first;
        while (p != NULL) {
            f(p);
            p = p->next;
        }
    }

    screen_obj *findlast(bool (*P)(screen_obj *))
    {
        screen_obj *p = first;
        screen_obj *t = NULL;

        while (p != NULL) {
            if (P(p))
                t = p;
            p = p->next;
        }
        return t;
    }

    screen_obj *remove(screen_obj *o)
    {
        screen_obj *p = first;
        if (o == NULL)
            return NULL;
        if (p == NULL)         // no elements
            return NULL;
        if (p->next == NULL) { // one element
            if (p != o)
                return NULL;
            first = NULL;
            return p;
        }
        if (p == o) {          // o is first element
            first = p->next;
            p->next = NULL;
            return p;
        }
        while ((p->next != o) && (p->next != NULL)) {
            p = p->next;
        }
        if (p->next == o) {
            p->next = o->next;
            o->next = NULL;
            return o;
        }
        return NULL;
    }
};

list screen_list;

const int type_particle = 4;
struct particle : screen_obj
{
    int x;
    int y;
    int vx;
    int vy;
    int colour;
    int radius;
    int ttl = 90;

    particle() {}
    particle(int x, int y, int vx, int vy, int color, int r) : x(x), y(y), vx(vx), vy(vy), colour(color), radius(r) {}

    virtual bool testxy(int x, int y)
    {
        return (x - this->x)*(x - this->x) + (y - this->y)*(y - this->y) <= radius*radius;
    }

    virtual void clear()
    {
        //if (rand()&1) vy++;
        //else vy--;
        if (x >= 800 - 1 - radius) x  = 800 - 1 - radius;
        if (x <= radius)       x  = radius;
        if (y >= 600 - 1 - radius) y  = 600 - 1 - radius;
        if (y <= radius)       y  = radius;
        PutCircleNewC(x, y, 0, radius);
    }

    virtual void animate()
    {
        clear();
        x += vx;
        if (ttl % 2 == 0)
            radius--;
        int t = 0;
        if (radius == 0) {
            screen_list.remove(this);
            t = 1;
        }
        if (ttl == 0) {
            screen_list.remove(this);
            t = 1;
        }
        if (x >= 800 - 1 - radius) {
            screen_list.remove(this);
            t = 1;
        }
        if (x <= radius) {
            screen_list.remove(this);
            t = 1;
        }
        y += vy;
        if (y >= 600 - 1 - radius) {
            screen_list.remove(this);
            t = 1;
        }
        if (y <= radius) {
            screen_list.remove(this);
            t = 1;
        }
        rad_grad_param p1 = { GREEN, YELLOW };
        if (t == 0)
            PutTexturedCircle(x, y, radius, RadGradTex, &p1);
        ttl--;
    }
};
particle* particle_create(circle *p);

//
// CIRCLE
//

circle* circle_create()
{
    circle *pc = new circle();
    if (pc) {
        pc->type = type_circle;
        pc->x = mouse.x;
        pc->y = mouse.y;
        pc->vx = (unsigned) rand() % 10 - 5;
        pc->vy = (unsigned) rand() % 10 - 5;
        pc->colour = rand();
        pc->radius = (unsigned) rand() % 60 + 10;
    }
    return pc;
}

//
// STRING
//

str* str_create()
{
    str *ps = new str();
    if (ps) {
        ps->type = type_str;
        ps->x = mouse.x;
        ps->y = mouse.y;
        ps->vx = (unsigned) rand() % 30 - 15;
        ps->vy = (unsigned) rand() % 30 - 15;
        ps->color = rand();
        int r = rand() & 1;
        if (r) ps->str = "Nagetsi";
        else ps->str = "Vareniki";
        ps->len = strlen(ps->str);
    }
    return ps;
}

//
// PARTICLE
//

particle* particle_create(circle *p)
{
    particle *pc = new particle();
    if (pc) {
        pc->type = type_particle;
        pc->x = p->x;
        pc->y = p->y;
        pc->vx = p->vx / 2 + (unsigned) rand() % 10 - 5;
        pc->vy = p->vy / 2 + (unsigned) rand() % 5 - 2;
        pc->colour = rand();
        pc->radius = p->radius - (unsigned) rand() % p->radius;
    }
    return pc;
}

//
// ANIMATE
//

star star1c(200, 100, 8, 6);
star star2c(300, 500, 1, 1);

bool Ptrue(screen_obj *){ return true; }
bool test_mouse(screen_obj *p) { return p->testxy(mouse.x, mouse.y); }
void animate(screen_obj *p) { p->animate(); }

void asm_main_text(void)
{
    srand(0);
    PutStrC(100, 100, 0xff1155, "Entaro Adun!");

    circle *pc = circle_create();
    printf("screen_list.first=%p\n", screen_list.first);
    int pause = 0;

    while (1) {
        int k = waitkey();
        mouse.x = (k >> 9) & 0x7FF; // 111'1111'1111 = 0x7FF
        mouse.y = (k >> 20) & 0x7FF; // 111'1111'1111 = 0x7FF
        int e = k & 0x1FF;

        switch (e) {
        case ' ':
            pause = !pause;
            break;
        case 'l':
            printf("the last is %p\n", screen_list.findlast([](screen_obj *){return true;}));
            break;
        case Key::MouseMove:
            // hnhmh
            break;
        case Key::MouseLeft:
            screen_list.add(circle_create());
            break;
        case Key::MouseRight:
            screen_list.add(str_create());
            printf("NAGETSI\n");
            break;
        case Key::MouseMiddle: {
            screen_obj *p = screen_list.findlast(test_mouse);
            p = screen_list.remove(p);
            if (p != NULL) {
                p->clear();
                if (p->type == type_circle) {
                    for (int i = 0; i < 20; i++)
                        screen_list.add(particle_create((circle *)p));
                }
            }
            break;
        }
        case Key::NextFrame:
            if (!pause) {
                screen_list.walk(animate);
                star1c.animate();
                star2c.animate();
                PutPix(400, 300, 0xffff00);
                PutCircleNewC(100, 100, 0xff0000, 20);
                pc->animate();
            }
            break;
        }
    }
}
