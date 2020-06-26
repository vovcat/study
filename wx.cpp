// i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc test_wx.cpp -o test_wx.exe -lgdi32 && wine ./test_wx.exe
// g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_wx.cpp -o test_wx -lX11 -lXext -lpthread && ./test_wx

#define _WIN32_WINNT 0x0601 // Windows 7

#include <stdio.h> // printf()
#include <unistd.h> // usleep()
#include <string.h> // strstr()

#include <algorithm> // std::min()
#include <string>
#include <chrono>

#define THIS_CLASSNAME "VirtualMachine"
#define THIS_RESNAME "virtualmachine"
#define THIS_TITLE "Virtual Machine"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif

// asm

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
    int getkey(void);
    int waitkey(void);
    void asm_main_text(void);
}

//#include "wxasm.cpp"

// getkey()

#if defined(linux)

#include <queue>
#include <mutex> // std::mutex
#include <condition_variable> // std::condition_variable
#include <thread> // std::unique_lock

template<typename T> struct cqueue
{
    std::queue<T> q;
    mutable std::mutex m;
    std::condition_variable cv;
    typedef std::unique_lock<std::mutex> lock_t;
    size_t size() const { lock_t l(m); return q.size(); }
    T get() { lock_t l(m); while (q.empty()) cv.wait(l); T r = q.front(); q.pop(); return r; }
    void put(const T v) { lock_t l(m); q.push(v); l.unlock(); cv.notify_one(); }
};

void getkey_stop_thread(void)
{
    pthread_exit(NULL);
}

#elif defined(WIN32)

#include <queue>
#include <windows.h>

template<typename T> struct cqueue
{
    std::queue<T> q;
    mutable CRITICAL_SECTION m;
    CONDITION_VARIABLE cv;
    size_t size() const { EnterCriticalSection(&m); auto sz = q.size(); LeaveCriticalSection(&m); return sz; }
    T get() { EnterCriticalSection(&m); while (q.empty()) SleepConditionVariableCS(&cv, &m, INFINITE); T r = q.front(); q.pop(); LeaveCriticalSection(&m); return r; }
    void put(const T v) { EnterCriticalSection(&m); q.push(v); LeaveCriticalSection(&m); WakeConditionVariable(&cv); }
    cqueue() { InitializeCriticalSection(&m); InitializeConditionVariable(&cv); }
};

void getkey_stop_thread(void)
{
    ExitThread(0);
}

#endif

const unsigned FPS = 60;
const unsigned FPS_DELAY = 1000 / FPS - 1;
const int keydelay = 3900; //us
bool getkey_down = false;
cqueue<int> getkey_q;

framebuf_t framebuf;
framebuf_t *pframebuf = &framebuf;

int waitkey(void)
{
    if (getkey_down) { getkey_stop_thread(); return 0; }
    if (getkey_q.size() == 0) usleep(keydelay);
    return getkey_q.get();
}

int getkey(void)
{
    if (getkey_down) { getkey_stop_thread(); return 0; }
    if (getkey_q.size() == 0) usleep(keydelay);
    if (getkey_q.size() == 0) return Key::Nokey;
    return getkey_q.get();
}

void getkey_stop(void)
{
    getkey_down = true;
    getkey_q.put(Key::Nokey); // wake up reader thread
    usleep(keydelay * 2); // wait for possible usleep() there
}

//
// MAIN: linux
//

#if defined(linux)

#include <stdlib.h> // aligned_alloc()
#include <unistd.h> // usleep(), sysconf()
#include <time.h> // time(), nanosleep()
#include <locale.h> // setlocale()
#include <pthread.h> // pthread_cancel()
#include <signal.h> // sigaction(), SA_RESTART, SIGALRM, SIG_DFL
#include <sys/ipc.h> // IPC_PRIVATE, IPC_CREAT
#include <sys/shm.h> // shmget(), shmat()
#include <sys/time.h> // setitimer(), gettimeofday()
#include <sys/mman.h> // mmap(), munmap()

#include <X11/Xlib.h> // Display, Visual, X*
#include <X11/Xlocale.h> // XSetLocaleModifiers(), XSupportsLocale()
#include <X11/Xatom.h> // XA_STRING, XA_*
#include <X11/Xutil.h> // XTextProperty, XSizeHints, XSetWMProperties(), XVisualInfo
#include <X11/XKBlib.h> // XkbKeycodeToKeysym()
#include <X11/extensions/XShm.h> // XShmSegmentInfo, XShmPixmapFormat(), XShmCreateImage(), XShmAttach(), XShmDetach(), XShmPutImage()
#include <X11/extensions/Xdbe.h> // XdbeQueryExtension()

// UTILITES

#define InternAtom(x) do { XA_##x = XInternAtom(display, #x, False); } while (0)
#define DefineAtom(x) Atom XA_##x = XInternAtom(display, #x, False); (void)XA_##x
#define DefineAtomValue(x, v) Atom XA_##x = XInternAtom(display, v, False); (void)XA_##x

Display *display;

static void RedrawWindow(Display *display, Window win, int count = 0)
{
    XExposeEvent ev = { Expose, 0, True, display, win, 0, 0, FB_WIDTH, FB_HEIGHT, count };
    XSendEvent(display, win, False, 0 /*event_mask*/, (XEvent *) &ev);
}

// TIMER

static Display *timer_dpy = NULL;
static Window timer_win = 0;
static Atom timer_atom = 0;
static struct sigaction timer_sa = {};

static void timer_alarm(int/*sig*/, siginfo_t */*si*/, void */*ucontext*/)
{
    if (!timer_dpy || !timer_win) return;
    RedrawWindow(timer_dpy, timer_win, timer_atom);
    XFlush(timer_dpy);
}

long timer_start(Display *dpy, Window win, Atom atom, int ms)
{
    timer_dpy = dpy;
    timer_win = win;
    timer_atom = atom;

    struct sigaction sa = {};
    sa.sa_flags = SA_RESTART | SA_SIGINFO;
    sa.sa_sigaction = timer_alarm;//(int sig, siginfo_t *si, void *ucontext)
    if (sigaction(SIGALRM, &sa, &timer_sa) < 0) {
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }

    struct itimerval it = {};
    it.it_value.tv_sec = it.it_interval.tv_sec = ms / 1000;
    it.it_value.tv_usec = it.it_interval.tv_usec = (ms % 1000) * 1000;
    if (setitimer(ITIMER_REAL, &it, NULL) < 0) {
        perror("setitimer()");
        exit(EXIT_FAILURE);
    }

    return 0;
}

void timer_stop(long)
{
    struct itimerval it = {};
    if (setitimer(ITIMER_REAL, &it, NULL) < 0) {
        perror("setitimer()");
        exit(EXIT_FAILURE);
    }
    if (sigaction(SIGALRM, &timer_sa, NULL) < 0) {
        perror("sigaction()");
        exit(EXIT_FAILURE);
    }
    timer_win = None;
    timer_dpy = NULL;
}

// MAIN

void asm_main_call(void)
{
    pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, NULL);
    //asm volatile ("call asm_main" ::: "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");
    asm_main_text();
    //return NULL;
}

int main(int argc, char *argv[])
{
    int screen, depth, bitmap_pad;
    Window defrootwin, rootwin;
    Visual *visual;
    unsigned long allplanes_mask;
    int ShmCompletionEvent = 0; // event type
    bool use_shared_pixmaps = 1;
    bool use_dbe = 1;

    /* GENERAL INFO */

    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) perror("sysconf"), exit(1);

    char *locale = setlocale(LC_ALL, "");
    if (locale == NULL) perror("setlocale"), exit(1);

    printf("pagesize = %d\n", pagesize); // 4096
    printf("locale = %s\n", locale);
    printf("XInitThreads() = %d\n", XInitThreads()); // 1
    printf("XOpenDisplay = %p\n", display = XOpenDisplay(NULL));

    printf("ScreenCount = %d\n", XScreenCount(display)); // 1
    printf("DefaultScreen = %d\n", screen = XDefaultScreen(display)); // 0
    printf("DefaultRootWindow = %#lx\n", defrootwin = XDefaultRootWindow(display)); // 0x33a
    printf("DefaultVisual = %p\n", visual = DefaultVisual(display, screen)); // 0x125e3e0
    printf("RootWindow = %#lx\n", rootwin = XRootWindow(display, screen)); // 0x33a
    printf("DefaultDepth = %d\n", depth = XDefaultDepth(display, screen)); // 24
    printf("DisplayCells = %d\n", XDisplayCells(display, screen)); // 256
    printf("AllPlanes = %lx\n", allplanes_mask = XAllPlanes()); // 0xffffffff
    printf("DisplayPlanes = %d\n", XDisplayPlanes(display, screen)); // 24
    printf("DisplayString = %s\n", XDisplayString(display)); // :0.0
    printf("ServerVendor = %s\n", XServerVendor(display)); // The X.Org Foundation
    printf("VendorRelease = %d\n", XVendorRelease(display)); // 11906000
    printf("QLength = %d\n", XQLength(display)); // 0
    printf("ProtocolVersion = %d\n", XProtocolVersion(display)); // 11
    printf("ProtocolRevision = %d\n", XProtocolRevision(display)); // 0
    printf("MaxRequestSize = %ld\n", XMaxRequestSize(display)); // 65535
    printf("ExtendedMaxRequestSize = %ld\n", XExtendedMaxRequestSize(display)); // 4194303
    printf("ImageByteOrder = %d\n", XImageByteOrder(display)); // 0
    printf("BitmapUnit = %d\n", XBitmapUnit(display)); // 32
    printf("BitmapBitOrder = %d\n", XBitmapBitOrder(display)); // 0
    printf("BitmapPad = %d\n", bitmap_pad = XBitmapPad(display)); // 32
    printf("DisplayHeight = %d\n", XDisplayHeight(display, screen)); // 800
    printf("DisplayHeightMM = %d\n", XDisplayHeightMM(display, screen)); // 225
    printf("DisplayWidth = %d\n", XDisplayWidth(display, screen)); // 1280
    printf("DisplayWidthMM = %d\n", XDisplayWidthMM(display, screen)); // 361
    printf("XShmGetEventBase = %d\n", ShmCompletionEvent = XShmGetEventBase(display)); // 65

    /* ATOMS */

    DefineAtom(NULL);
    DefineAtom(WM_DELETE_WINDOW);
    DefineAtom(_NET_WM_PING);

    /* WINDOW PART */

    XSetWindowAttributes wattr = {};
    wattr.background_pixel = XBlackPixel(display, XDefaultScreen(display));
    wattr.bit_gravity = NorthWestGravity;
    wattr.win_gravity = NorthWestGravity;
    wattr.backing_store = NotUseful; // NotUseful, WhenMapped, Always
    wattr.save_under = False;

    Window win = XCreateWindow(display, XDefaultRootWindow(display),
        0 /*initX*/, 0 /*initY*/, FB_WIDTH /*sizeX*/, FB_HEIGHT /*sizeY*/, 0 /*border_width*/,
        CopyFromParent /*depth*/, InputOutput /*class*/, CopyFromParent /*visual*/,
        CWBackPixel | CWBitGravity | CWWinGravity | CWBackingStore | CWSaveUnder,
        &wattr
    );

    XSizeHints size_hints = {};
    size_hints.flags = PResizeInc | PMinSize | PMaxSize | PBaseSize | PAspect;
    size_hints.width_inc = 0;
    size_hints.height_inc = 0;
    size_hints.min_width = FB_WIDTH;
    size_hints.min_height = FB_HEIGHT;
    size_hints.max_width = FB_WIDTH;
    size_hints.max_height = FB_HEIGHT;
    size_hints.base_width = FB_WIDTH;
    size_hints.base_height = FB_HEIGHT;
    size_hints.min_aspect.x = FB_WIDTH;
    size_hints.min_aspect.y = FB_HEIGHT;
    size_hints.max_aspect.x = FB_WIDTH;
    size_hints.max_aspect.y = FB_HEIGHT;
    XSetWMNormalHints(display, win, &size_hints);

    // This function has been superseded by XSetWMProperties()
    // XSetStandardProperties(display, w, window_name, icon_name, icon_pixmap, argv, argc, hints)
    const char *name = THIS_TITLE;
    XSetStandardProperties(display, win, name, name/*icon_name*/, None/*icon_pixmap*/, argv, argc, &size_hints);

    XWMHints wm_hints = {};
    wm_hints.flags = InputHint | StateHint;
    wm_hints.input = True;
    wm_hints.initial_state = NormalState;

    XClassHint class_hints = {};
    class_hints.res_name = (char *) THIS_RESNAME;
    class_hints.res_class = (char *) THIS_CLASSNAME;

    unsigned char name_uc[] = THIS_TITLE;
    XTextProperty name_prop = { name_uc, XA_STRING, 8, strlen((char *) name_uc) };
    XSetWMProperties(display, win, &name_prop, &name_prop, argv, argc, &size_hints, &wm_hints, &class_hints);

    Atom protocols[] = { XA_WM_DELETE_WINDOW, XA__NET_WM_PING, };
    XSetWMProtocols(display, win, protocols, ARRAY_SIZE(protocols));

    // Clear backgroud, it looks more pleasant before the redraw event does its job
    XSetWindowBackground(display, win, 0x333333); // dark gray

    // We want events associated with the specified event mask
    unsigned long mask =
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask |
        PointerMotionMask |
        Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask |
        ButtonMotionMask |
        KeymapStateMask |
        ExposureMask |
        VisibilityChangeMask |
        StructureNotifyMask |
        ResizeRedirectMask |
        SubstructureNotifyMask |
        SubstructureRedirectMask |
        FocusChangeMask |
        PropertyChangeMask |
        ColormapChangeMask |
        OwnerGrabButtonMask |
        0;

    XSelectInput(display, win, mask);

    /* Default locale */

    XIM xim = NULL;
    XIC xic = NULL;
    printf("XSetLocaleModifiers() = '%s'\n", XSetLocaleModifiers(""));
    printf("XSupportsLocale() = %d\n", XSupportsLocale());
    printf("XOpenIM() = %p\n", xim = XOpenIM(display, NULL, NULL, NULL));
    printf("XLocaleOfIM() = %s\n", xim ? XLocaleOfIM(xim) : "null im");
    printf("XCreateIC() = %p\n", xic = XCreateIC(xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, XNClientWindow, win, XNFocusWindow, win, NULL));

    /* MAIN */

    XdbeBackBuffer dbe = None;
    if (use_dbe) {
        int dbe_ext = 0, dbe_major = 0, dbe_minor = 0;
        dbe_ext = XdbeQueryExtension(display, &dbe_major, &dbe_minor);
        if (dbe_ext) {
            dbe = XdbeAllocateBackBufferName(display, win, XdbeCopied);
            if (!dbe) dbe_ext = 0;
        }
    }

    XFontStruct *fnt = XLoadQueryFont(display, "6x10");

    XGCValues gcv = {};
    gcv.font = fnt->fid;
    gcv.foreground = XWhitePixel(display, screen);
    GC gc = XCreateGC(display, win, GCFont | GCForeground, &gcv);

    int mitshm_major = 0, mitshm_minor = 0, shared_pixmaps = use_shared_pixmaps;
    XShmSegmentInfo shminfo = { 0UL, -1, (char *) -1, False }; // must have a lifetime at least as long as that of the shmimg
    XImage *shmimg = NULL;

    if (shared_pixmaps && !XShmQueryExtension(display)) {
        shared_pixmaps = 0;
    }
    if (shared_pixmaps && !XShmQueryVersion(display, &mitshm_major, &mitshm_minor, &shared_pixmaps)) {
        shared_pixmaps = 0;
    }
    if (shared_pixmaps) {
        shmimg = XShmCreateImage(display, visual, depth, XShmPixmapFormat(display), NULL, &shminfo, FB_WIDTH, FB_HEIGHT);
        if (!shmimg) shared_pixmaps = 0;
    }
    if (shared_pixmaps) {
        int shmid = shmget(IPC_PRIVATE, shmimg->bytes_per_line * shmimg->height, IPC_CREAT | 0666);
        if (shmid != -1) shminfo.shmid = shmid;
        else shared_pixmaps = 0;
    }
    if (shared_pixmaps) {
        char *shmaddr = shmimg->data = (char *) shmat(shminfo.shmid, 0, 0);
        if (shmaddr != (char *) -1) shminfo.shmaddr = shmaddr;
        else shared_pixmaps = 0;
    }
    if (shared_pixmaps) {
        shminfo.readOnly = False;
        shared_pixmaps = XShmAttach(display, &shminfo); // attach to the shared memory segment
    }
    if (shared_pixmaps) {
        shared_pixmaps = XSync(display, False);
    }
    if (!shared_pixmaps) {
        XShmDetach(display, &shminfo);

        if (shminfo.shmaddr != (char *) -1) shmdt(shminfo.shmaddr);
        shminfo.shmaddr = (char *) -1;
        shmimg->data = NULL;

        if (shminfo.shmid != -1) shmctl(shminfo.shmid, IPC_RMID, 0);
        shminfo.shmid = -1;

        if (shmimg != NULL) XDestroyImage(shmimg);
        shmimg = NULL;
    }
    if (shmimg == NULL) {
        shmimg = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, FB_WIDTH, FB_HEIGHT, bitmap_pad, 0);
        if (shmimg) {
            shmimg->data = (char *) aligned_alloc(pagesize, shmimg->bytes_per_line * shmimg->height);
            if (!shmimg->data) { XDestroyImage(shmimg); shmimg = NULL; }
        }
        shared_pixmaps = 0;
    }
    if (shmimg)
        pframebuf = (framebuf_t *) shmimg->data;

    // Show the window
    XMapRaised(display, win); // -> Expose event
    XSync(display, false);

    // Start asm thread
    std::thread gThread(asm_main_call);
    gThread.detach();

    sigset_t sigmask = {};
    sigaddset(&sigmask, SIGALRM);
    pthread_sigmask(SIG_BLOCK, &sigmask, NULL); // serve SIGALRM on the asm thread

    // Start the framebuffer refresh timer (vsync-like)
    long timer = timer_start(display, win, XA_NULL, FPS_DELAY);

    // Main loop
    bool done = 0;
    int nloops = -1;
    int nredraws = 0;
    auto start = std::chrono::high_resolution_clock::now();

    while (!done) {
        XEvent ev;
        XNextEvent(display, &ev);
        switch (ev.type) {
            case Expose: {
                Drawable w = dbe ? dbe : win;
                if (shared_pixmaps) XShmPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT, 1/*send_event*/);
                else XPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT);
                nredraws++;
                break;
            }
            case KeyPress:
            case KeyRelease: {
                KeySym keysym = 0;
                Status status_return;
                unsigned char str[32] = {};
                int nbytes;

                if (ev.type == KeyPress) {
                    nbytes = XmbLookupString(xic, &ev.xkey, (char *) str, sizeof(str), &keysym, &status_return);
                } else {
                    XEvent ev1 = ev;
                    ev1.type = KeyPress;
                    nbytes = XmbLookupString(xic, &ev1.xkey, (char *) str, sizeof(str), &keysym, &status_return);
                }

                int key = 0;

                if (ev.type == KeyPress) {
                    if (nbytes == 1) {
                        key = str[0] ? str[0] : int(Key::Nil);
                    } else {
                        static const struct { int key; KeySym xkey; } keymapXK[] = {
                            {Key::F1,XK_F1}, {Key::F2,XK_F2}, {Key::F3,XK_F3}, {Key::F4,XK_F4}, {Key::F5,XK_F5}, {Key::F6,XK_F6}, {Key::F7,XK_F7}, {Key::F8,XK_F8}, {Key::F9,XK_F9}, {Key::F10,XK_F10}, {Key::F11,XK_F11}, {Key::F12,XK_F12},
                            {Key::ScrollLock,XK_Scroll_Lock}, {Key::Pause,XK_Pause}, {Key::Ins,XK_Insert}, {Key::Del,XK_Delete}, {Key::CapsLock,XK_Caps_Lock},
                            {Key::ShiftL,XK_Shift_L}, {Key::ShiftR,XK_Shift_R}, {Key::ControlL,XK_Control_L}, {Key::WinL,XK_ISO_Level3_Shift}, {Key::AltL,XK_Alt_L}, {Key::AltR,XK_Alt_R}, {Key::Menu,XK_Menu}, {Key::ControlR,XK_Control_R},
                            {Key::Home,XK_Home}, {Key::Left,XK_Left}, {Key::Up,XK_Up}, {Key::Right,XK_Right}, {Key::Down,XK_Down}, {Key::PgUp,XK_Page_Up}, {Key::PgDn,XK_Page_Down}, {Key::End,XK_End},
                        };
                        for (size_t i = 0; i < ARRAY_SIZE(keymapXK); i++) {
                            if (keysym == keymapXK[i].xkey) {
                                key = keymapXK[i].key;
                                break;
                            }
                        }
                    }
                    if (key) getkey_q.put(key);
                }

                done = key == 'q';
                break;
            }
            case MotionNotify: {
                XMotionEvent *xev = &ev.xmotion;
                int key = Key::MouseMove | (xev->x & 2047) << 9 | (xev->y & 2047) << 20;
                getkey_q.put(key);
                break;
            }
            case ButtonPress:
            case ButtonRelease: {
                XButtonEvent *xev = &ev.xbutton;
                int key = 0;
                if (ev.type == ButtonPress) {
                    XGrabPointer(display, win, true, ButtonMotionMask, GrabModeAsync, GrabModeAsync, win, None, xev->time);
                    key = Key::MouseLeft;
                } else {
                    XUngrabPointer(display, xev->time);
                    key = Key::MouseRelLeft;
                }
                key = ((key + xev->button - 1) & 511) | (xev->x & 2047) << 9 | (xev->y & 2047) << 20;
                getkey_q.put(key);
                break;
            }
            default: {
                if (ev.type == ShmCompletionEvent) {
                    if (dbe) {
                        XdbeSwapInfo si = { .swap_window = win, .swap_action = XdbeCopied };
                        XdbeSwapBuffers(display, &si, 1);
                    }
                }
                break;
            }
        }

        if (ev.type == KeyRelease && (ev.xkey.keycode == 54/*'c'*/ or ev.xkey.keycode == 45/*'k'*/)) {
            nredraws = 0;
            if (ev.xkey.keycode == 54/*'c'*/) nloops = 100;
            else nloops = 1000;
            RedrawWindow(display, win);
            start = std::chrono::high_resolution_clock::now();
        }
        if (ev.type == Expose) {
            if (nloops > 0) {
                RedrawWindow(display, win);
            } else if (nloops > -1) {
                std::chrono::duration<double, std::micro> ti = std::chrono::high_resolution_clock::now() - start;
                printf("=== %d redraws in %.2fs = %.2f draw/s\n", nredraws, ti.count() / 1e6, 1e6 * nredraws / ti.count());
            } if (nloops > -2) {
                nloops--;
            }
        }
        if (XEventsQueued(display, QueuedAfterReading) == 0) {
            // idle processing
        }
    }

    timer_stop(timer);
    XSync(display, true);
    getkey_stop(); // stop reader thread
    pthread_cancel(gThread.native_handle());
    pthread_join(gThread.native_handle(), NULL);
    XFreeGC(display, gc);
    XCloseIM(xim);
    XDestroyWindow(display, win);
    XCloseDisplay(display);
    return 0;
}

//
// MAIN: windows
//

#elif defined(WIN32)

#define IDT_TIMER1 0x14001101
#define IDT_TIMER2 0x14001102

#define ID_ABOUT 2000
#define ID_EXIT 2001

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

HWND gWnd = 0;
HANDLE gThread = 0;
static BOOL gModalState = false; // Is messagebox shown

static int debug = 0;
static int benchmark = 0;
int nredraws = 0;
int start = 20;

asm volatile (R"(
    # export to wxasm.cpp
    .global pframebuf
    .global waitkey
    .global getkey
    pframebuf = _pframebuf
    waitkey = _waitkey
    getkey = _getkey
    # import from wxasm.cpp
    .global _asm_main
    _asm_main = asm_main
)");

DWORD WINAPI asm_main_call(void *)
{
    //asm volatile ("call asm_main" ::: "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");
    asm_main_text();
    return 0;
}

// windows: 1ms resolution set by timeBeginPeriod(1)
int usleep(useconds_t usec)
{
    static HANDLE timer = NULL;
    LARGE_INTEGER ft;
    ft.QuadPart = -10LL * usec; // Convert to 100 nanosecond interval, negative value indicates relative time
    if (!timer) timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    //CloseHandle(timer);
    return 0;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static HDC gdcMem;
    static HBITMAP gBitmap;
    static UINT_PTR gTimer1;
    static RECT rcClient; // client area rectangle

    switch (uMsg) {
        case WM_CREATE: {
            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = FB_WIDTH;
            bmi.bmiHeader.biHeight = -FB_HEIGHT; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 8*4;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = ((((FB_WIDTH * bmi.bmiHeader.biBitCount) + 31) & ~31) >> 3) * FB_HEIGHT;

            HDC hdc = GetDC(hWnd);
            gdcMem = CreateCompatibleDC(hdc); // Temp HDC to copy picture
            gBitmap = CreateDIBSection(hdc, &bmi, DIB_RGB_COLORS, (void **)&pframebuf, NULL, 0);
            if (pframebuf) memcpy(pframebuf, &framebuf, sizeof(*pframebuf));
            HGDIOBJ old = SelectObject(gdcMem, gBitmap);
            DeleteObject(old); // release the 1x1@1bpp default bitmap
            ReleaseDC(hWnd, hdc);

            DWORD tid;
            gThread = CreateThread(NULL, 0, asm_main_call, NULL, 0, &tid);
            gTimer1 = SetTimer(hWnd, IDT_TIMER1, FPS_DELAY, (TIMERPROC) NULL);
            return 0;
        }
        case WM_CLOSE: {
            if (MessageBox(hWnd, "Really quit?", TEXT(THIS_TITLE), MB_OKCANCEL) != IDOK) {
                // User canceled. Do nothing.
                return 0;
            }
            break;
        }
        case WM_DESTROY: {
            if (gTimer1) {
                if (gTimer1) KillTimer(hWnd, gTimer1);
                gTimer1 = 0;
            }

            if (gThread) {
                TerminateThread(gThread, 1);
                CloseHandle(gThread);
                gThread = 0;
            }

            pframebuf = &framebuf;
            gWnd = 0;

                DeleteObject(gBitmap);
                DeleteDC(gdcMem);

            PostQuitMessage(0);
            return 0;
        }
        case WM_PAINT: {
            PAINTSTRUCT ps = {};
            HDC hdc = BeginPaint(hWnd, &ps);
            BitBlt(hdc, 0, 0, FB_WIDTH, FB_HEIGHT, gdcMem, 0, 0, SRCCOPY);
            EndPaint(hWnd, &ps);
            nredraws++;
            return 0;
        }
        case WM_ERASEBKGND: {
            return 1;
        }
        case WM_MOVE:
        case WM_SIZE: {
            GetClientRect(hWnd, &rcClient);
            break;
        }

        case WM_MOUSEMOVE: {
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            int key = Key::MouseMove | (x & 2047) << 9 | (y & 2047) << 20;
            getkey_q.put(key);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            int delta = int(wParam) >> 16;
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

            int key = (x & 2047) << 9 | (y & 2047) << 20;
            if (delta < 0) { getkey_q.put(Key::MousePgDn | key); getkey_q.put(Key::MouseRelPgDn | key); }
            else { getkey_q.put(Key::MousePgUp | key); getkey_q.put(Key::MouseRelPgUp | key); }
            return 0;
        }
        case WM_LBUTTONDBLCLK:
        case WM_RBUTTONDBLCLK:
        case WM_MBUTTONDBLCLK:
        case WM_XBUTTONDBLCLK:
        case WM_XBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONDOWN: {
            int state = GET_KEYSTATE_WPARAM(wParam);
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

            int key = uMsg == WM_LBUTTONDOWN ? Key::MouseLeft :
                    uMsg == WM_MBUTTONDOWN ? Key::MouseMiddle :
                    uMsg == WM_RBUTTONDOWN ? Key::MouseRight :
                    uMsg == WM_XBUTTONDOWN ? Key::MouseX1 : 0;
            if (!key) key = state == 1 ? Key::MouseLeft :
                    state == 2 ? Key::MouseRight :
                    state == 0x10 ? Key::MouseMiddle : 0;
            if (key) getkey_q.put(key | (x & 2047) << 9 | (y & 2047) << 20);

            // Restrict the mouse cursor to the client area.
            // This ensures that the window receives a matching WM_LBUTTONUP message.
            if (state) ClipCursor(&rcClient);
            return 0;
        }
        case WM_XBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:
        case WM_LBUTTONUP: {
            int state = GET_KEYSTATE_WPARAM(wParam);
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

            int key = uMsg == WM_LBUTTONUP ? Key::MouseRelLeft :
                    uMsg == WM_MBUTTONUP ? Key::MouseRelMiddle :
                    uMsg == WM_RBUTTONUP ? Key::MouseRelRight :
                    uMsg == WM_XBUTTONUP ? Key::MouseRelX1 : 0;
            if (key) getkey_q.put(key | (x & 2047) << 9 | (y & 2047) << 20);

            // Release the mouse cursor
            if (!state) ClipCursor(NULL);
            return 0;
        }

        case WM_SYSKEYUP:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_KEYDOWN: {
            const int vk = wParam;
            const int scan = (lParam >> 16) & 0x00ff;
            const int extKey = (lParam >> 24) & 1;

            BYTE keyboardState[256] = {};
            GetKeyboardState(keyboardState);

            WORD ascii = 0;
            int alen = ToAscii(vk, scan, keyboardState, &ascii, 0);

            int key = 0;

            if (uMsg == WM_KEYDOWN || uMsg == WM_SYSKEYDOWN) {
                if (alen == 1 && ascii < 0x100) {
                    key = ascii ? ascii : int(Key::Nil);
                } else {
                    static const struct { int key; int vk; } keymapVK[] = {
                        {Key::F1,VK_F1}, {Key::F2,VK_F2}, {Key::F3,VK_F3}, {Key::F4,VK_F4}, {Key::F5,VK_F5}, {Key::F6,VK_F6}, {Key::F7,VK_F7}, {Key::F8,VK_F8}, {Key::F9,VK_F9}, {Key::F10,VK_F10}, {Key::F11,VK_F11}, {Key::F12,VK_F12},
                        {Key::ScrollLock,VK_SCROLL}, {Key::Pause,VK_PAUSE}, {Key::Ins,VK_INSERT}, {Key::Del,VK_DELETE}, {Key::CapsLock,VK_CAPITAL},
                        {Key::ShiftL,VK_SHIFT}, {Key::ShiftR,VK_RSHIFT}, {Key::ControlL,VK_CONTROL}, {Key::WinL,224/*VK_LWIN*/}, {Key::AltL,VK_MENU}, {Key::AltR,VK_RMENU}, {Key::Menu,VK_APPS}, {Key::ControlR,VK_RCONTROL},
                        {Key::Home,VK_HOME}, {Key::Left,VK_LEFT}, {Key::Up,VK_UP}, {Key::Right,VK_RIGHT}, {Key::Down,VK_DOWN}, {Key::PgUp,VK_PRIOR}, {Key::PgDn,VK_NEXT}, {Key::End,VK_END},
                    };
                    for (size_t i = 0; i < ARRAY_SIZE(keymapVK); i++) {
                        if (vk == keymapVK[i].vk) {
                            key = keymapVK[i].key;
                            break;
                        }
                    }
                    if (key == Key::ShiftL && scan == 54) key = Key::ShiftR;
                    else if (key == Key::ControlL && extKey == 1) key = Key::ControlR;
                    else if (key == Key::WinL && extKey == 1) key = Key::WinR;
                    else if (key == Key::AltL && extKey == 1) key = Key::AltR;
                }
                if (key) getkey_q.put(key);
            }

            if (key == 'q') DestroyWindow(hWnd); // Quit
            return 0;
        }

        case WM_TIMER: {
            switch (wParam) {
                case IDT_TIMER1:
                    // process the framebuffer refresh timer (vsync-like)
                    InvalidateRect(hWnd, NULL, false);
                    return 0;
            }
            break;
        }

        case WM_COMMAND: {
            if (gModalState)
                 return 1;
            switch (LOWORD(wParam)) {
                case ID_ABOUT:
                    gModalState = true;
                    MessageBox(hWnd, TEXT("Hi!"), TEXT("Title"), MB_ICONINFORMATION | MB_OK);
                    gModalState = false;
                    return 0;
                case ID_EXIT:
                    PostMessage(hWnd, WM_CLOSE, 0, 0);
                    return 0;
            }
            return 0;
        }

        case WM_APP: {
            switch (lParam) {
                case WM_LBUTTONDBLCLK:
                    MessageBox(hWnd, TEXT("Hi!"), TEXT("Title"), MB_ICONINFORMATION | MB_OK);
                    return 0;
                case WM_RBUTTONUP:
                    SetForegroundWindow(hWnd);
                    //ShowPopupMenu(hWnd, NULL, -1);
                    PostMessage(hWnd, WM_APP + 1, 0, 0);
                    return 0;
            }
            return 0;
        }
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Entry point

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int nCmdShow)
{
    HWND hPrev = NULL;
    int nloops = -1;

    if (strstr(lpCmdLine, "-d")) debug = 1;
    if (strstr(lpCmdLine, "-dd")) debug = 2;
    if (strstr(lpCmdLine, "-b")) { benchmark = 1; debug = 0; }
    if (strstr(lpCmdLine, "-c")) nloops = 100;
    if (strstr(lpCmdLine, "-k")) nloops = 1000;

    if ((hPrev = FindWindow(TEXT(THIS_CLASSNAME), TEXT(THIS_TITLE))) != 0)  {
        MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK);
        return 0;
    }

    // Initialize Winsock
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (res != 0) {
        printf("WSAStartup failed: %d\n", res);
        return 1;
    }

    WNDCLASSEX wclx = {};
    wclx.cbSize         = sizeof(wclx);
    wclx.style          = CS_HREDRAW | CS_VREDRAW;
    wclx.lpfnWndProc    = &WndProc;
    wclx.cbClsExtra     = 0;
    wclx.cbWndExtra     = 0;
    wclx.hInstance      = hInstance;
    wclx.hIcon          = LoadIcon(hInstance, IDI_APPLICATION);
    wclx.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wclx.hbrBackground  = NULL;
    wclx.hbrBackground  = CreateSolidBrush(0x333333);
    wclx.lpszMenuName   = NULL;
    wclx.lpszClassName  = TEXT(THIS_CLASSNAME);
    RegisterClassEx(&wclx);

    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD dwExStyle = 0; //WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_EX_NOACTIVATE;

    RECT clientarea = { 0, 0, FB_WIDTH, FB_HEIGHT };
    if (!AdjustWindowRectEx(&clientarea, dwStyle/*GetWindowLong(hWnd, GWL_STYLE)*/, 0/*GetMenu(hWnd) != 0*/, dwExStyle/*GetWindowLong(hWnd, GWL_EXSTYLE)*/)) {
        clientarea = { 0, 0, FB_WIDTH + 8, FB_HEIGHT + 27 };
    }

    HWND hWnd = gWnd = CreateWindowEx(
        dwExStyle,		// Extended possibilites for variation
        TEXT(THIS_CLASSNAME), // Classname
        TEXT(THIS_TITLE),	// Title Text
        dwStyle,		// default window
        CW_USEDEFAULT,      // Windows decides the position
        CW_USEDEFAULT,      // Where the window ends up on the screen
        clientarea.right - clientarea.left, // width and
        clientarea.bottom - clientarea.top, // height in pixels
        HWND_DESKTOP,       // The window is a child-window to desktop
        NULL,               // No menu
        hInstance,          // Program Instance handler
        NULL                // No Window Creation data
    );

    if (!hWnd) {
        MessageBox(NULL, "Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
        return 1;
    }

    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);

    ShowWindow(hWnd, nCmdShow);
    GdiFlush();
    UpdateWindow(hWnd);

    timeBeginPeriod(1); // minimum timer resolution, in milliseconds
    auto start = std::chrono::high_resolution_clock::now();

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);

        if (msg.message == WM_CHAR && (msg.wParam == 'c' || msg.wParam == 'K')) {
            nredraws = 0;
            if (msg.message == WM_CHAR && msg.wParam == 'K') {
                nloops = 1000;
            } else {
                nloops = 100;
            }
            InvalidateRect(hWnd, NULL, false);
            start = std::chrono::high_resolution_clock::now();
        }
        if (msg.message == WM_PAINT) {
            if (nloops > 0) {
                InvalidateRect(hWnd, NULL, false);
            } else if (nloops > -1) {
                std::chrono::duration<double, std::micro> ti = std::chrono::high_resolution_clock::now() - start;
                printf("=== %d redraws in %.2fs = %.2f draw/s\n", nredraws, ti.count() / 1e6, 1e6 * nredraws / ti.count());
                if (0) PostMessage(msg.hwnd, WM_DESTROY, 0, 0);
            } if (nloops > -2) {
                nloops--;
            }
        }
    }

    timeEndPeriod(1); // match each call to timeBeginPeriod with a call to timeEndPeriod, specifying the same minimum resolution
    UnregisterClass(TEXT(THIS_CLASSNAME), hInstance);
    return msg.wParam;
}

#endif
