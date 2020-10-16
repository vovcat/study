// i686-w64-mingw32-g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -mwindows -static-libgcc test_wx_all.cpp -o test_wx_all.exe -lgdi32 && wine ./test_wx_all.exe
// g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_wx_all.cpp -o test_wx_all -lX11 -lXext -lpthread && ./test_wx_all

// if (!IsRectEmpty(&rcTarget)) if (IntersectRect(&rcTmp, &rcBmp, &rcTarget)) if (PtInRect(&rcBmp, pt))
// CopyRect(&rcBmp, &rcTarget); SetRectEmpty(&rcTarget);
/*
Windows:
            method	1	2	3
    friksasha-dell	860	1660	1600
    deti		590	1350	1300
    hp			155	200	198

X11:
    !shm backing_store = Always === 1001 redraws in 11.24s = 89.04 draw/s
    !shm backing_store = NotUseful === 1001 redraws in 9.61s = 104.14 draw/s
     shm backing_store = Always === 1001 redraws in 5.78s = 173.28 draw/s
     shm backing_store = NotUseful === 1001 redraws in 3.93s = 254.46 draw/s
     shm backing_store = NotUseful +send_event === 1001 redraws in 3.93s = 254.60 draw/s
     shm backing_store = NotUseful +send_event +dbe === 1001 redraws in 5.79s = 172.77 draw/s
*/

#define _WIN32_WINNT 0x0601 // Windows 7

#include <inttypes.h> // PRIu64
#include <stdio.h> // printf()
#include <unistd.h> // usleep()
#include <string.h> // strstr(), memcmp()

#include <algorithm> // std::min()
#include <string>
#include <chrono>

#define THIS_CLASSNAME "VirtualMachine"
#define THIS_RESNAME "virtualmachine"
#define THIS_TITLE "Virtual Machine"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof(a) / sizeof((a)[0]))
#endif
#ifndef ARRAYSIZE
#define ARRAYSIZE(a) ARRAY_SIZE(s)
#endif

#ifndef PRId64
#define PRId64 "I64d"
#endif

#ifndef PRIu64
#define PRIu64 "I64u"
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

// Globals

static int debug = 0;
static int benchmark = 0;
static int nredraws = 0;

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

#include <windows.h>
#include <avrt.h>
#include <queue>

template<typename T> struct cqueue
{
    std::queue<T> q;
    mutable CRITICAL_SECTION m;
    CONDITION_VARIABLE cv;
    size_t size() const { EnterCriticalSection(&m); auto sz = q.size(); LeaveCriticalSection(&m); return sz; }
    T get() { EnterCriticalSection(&m); while (q.empty()) SleepConditionVariableCS(&cv, &m, INFINITE); T r = q.front(); q.pop(); LeaveCriticalSection(&m); return r; }
    void put(const T v) { EnterCriticalSection(&m); q.push(v); LeaveCriticalSection(&m); WakeConditionVariable(&cv); SwitchToThread(); }
    cqueue() { InitializeCriticalSection(&m); InitializeConditionVariable(&cv); }
};

void getkey_stop_thread(void)
{
    ExitThread(0);
}

#endif

static const struct {
    int key;
    const char *name;
} KeyNames[] = {
#define TAB(x) { x, #x }
    TAB(Key::Nil),
    TAB(Key::F1),
    TAB(Key::F2),
    TAB(Key::F3),
    TAB(Key::F4),
    TAB(Key::F5),
    TAB(Key::F6),
    TAB(Key::F7),
    TAB(Key::F8),
    TAB(Key::F9),
    TAB(Key::F10),
    TAB(Key::F11),
    TAB(Key::F12),
    TAB(Key::ScrollLock),
    TAB(Key::Pause),
    TAB(Key::Ins),
    TAB(Key::Del),
    TAB(Key::CapsLock),
    TAB(Key::ShiftL),
    TAB(Key::ShiftR),
    TAB(Key::ControlL),
    TAB(Key::WinL),
    TAB(Key::AltL),
    TAB(Key::AltR),
    TAB(Key::WinR),
    TAB(Key::Menu),
    TAB(Key::ControlR),
    TAB(Key::Home),
    TAB(Key::Left),
    TAB(Key::Up),
    TAB(Key::Right),
    TAB(Key::Down),
    TAB(Key::PgUp),
    TAB(Key::PgDn),
    TAB(Key::End),
    TAB(Key::MouseMove),
    TAB(Key::MouseLeft),
    TAB(Key::MouseMiddle),
    TAB(Key::MouseRight),
    TAB(Key::MousePgUp),
    TAB(Key::MousePgDn),
    TAB(Key::MouseX1),
    TAB(Key::MouseX2),
    TAB(Key::MouseRelLeft),
    TAB(Key::MouseRelMiddle),
    TAB(Key::MouseRelRight),
    TAB(Key::MouseRelPgUp),
    TAB(Key::MouseRelPgDn),
    TAB(Key::MouseRelX1),
    TAB(Key::MouseRelX2),
    TAB(Key::NextFrame),
#undef TAB
};

const char *KeyName(int key) {
    static char buf[4096] = { };
    static char *bufp = buf;
    int len;

    key &= 511;
    if (key < 1)
        return "Nokey";
    if (key < 32)
        len = sprintf(bufp, "^%c", key + '@');
    else if (key < 256)
        len = sprintf(bufp, "'%c'", key);
    else {
        for (size_t i = 0; i < ARRAY_SIZE(KeyNames); i++)
            if (KeyNames[i].key == key)
                return KeyNames[i].name;
        len = sprintf(bufp, "Key::Unknown(%d)'", key);
    }
    char *p = bufp;
    bufp += len + 1;
    return p;
}

int getkey_wait(int wait);

const unsigned FPS = 60;
const unsigned FPS_DELAY = 1000 / FPS - 1;
const int keydelay = 900; //us
bool getkey_down = false;
cqueue<int> getkey_q;

framebuf_t framebuf, oldframebuf;
framebuf_t *pframebuf = &framebuf;

int waitkey(void)
{
    if (getkey_down) { getkey_stop_thread(); return 0; }
    return getkey_wait(1);
    if (getkey_q.size() == 0) usleep(keydelay);
    return getkey_q.get();
}

int getkey(void)
{
    if (getkey_down) { getkey_stop_thread(); return 0; }
    return getkey_wait(0);
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

int getkey_wait(int wait)
{
    static int ncalls = 0;
    const auto min = std::chrono::high_resolution_clock::time_point::min();
    static auto firstcall = min;

    if (firstcall == min) firstcall = std::chrono::high_resolution_clock::now();
    ncalls++;

    //extern HWND gWnd;
    //if (gWnd) return SendMessage(gWnd, WM_USER, !!wait, 0);

    if (getkey_q.size() == 0) usleep(keydelay);
    if (!wait && getkey_q.size() == 0) return Key::Nokey;

    int key = getkey_q.get();
    std::chrono::duration<double, std::micro> ti = std::chrono::high_resolution_clock::now() - firstcall;
    if (debug) printf("=== key=%s %08x (%d calls in %.2fs = %.2f calls/s = %.2fus/call)\n", KeyName(key), key,
        ncalls, ti.count() / 1e6, 1e6 * ncalls / ti.count(), ti.count() / ncalls);
    return key;
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

#include "auto.h"
#include "debX11.cpp"
#include <fcntl.h> // open(), O_RDWR, O_CLOEXEC
#include <sys/ioctl.h> // ioctl()
#include <libdrm/drm.h> // drm_wait_vblank_t, DRM_IOCTL_WAIT_VBLANK
//#include <xf86drm.h> // drmVersion
//#include <xf86drmMode.h> // drmModeRes, drmModeGetResources(), drmModeGetConnector()

// UTILITES

#define InternAtom(x) do { XA_##x = XInternAtom(display, #x, False); } while (0)
#define DefineAtom(x) Atom XA_##x = XInternAtom(display, #x, False); (void)XA_##x
#define DefineAtomValue(x, v) Atom XA_##x = XInternAtom(display, v, False); (void)XA_##x

Display *display;

void cleanup_gc(GC *pgc) { if (display && *pgc) XFreeGC(display, *pgc); }
#define attr_cleanup_gc __attribute__((__cleanup__(cleanup_gc)))

void cleanup_pixmap(Pixmap *ppxm) { if (display && *ppxm) XFreePixmap(display, *ppxm); }
#define attr_cleanup_pixmap __attribute__((__cleanup__(cleanup_pixmap)))

void print_prop(Window win, Atom a)
{
    XTextProperty prop;

    if (!XGetTextProperty(display, win, &prop, a)) {
        DEBUGF("prop %T failed!", debX11_atom(a));
    } else {
        DEBUGF("prop %T encoding=%T format=%d [%lu]: '%.2s'", debX11_atom(a),
            debX11_atom(prop.encoding), prop.format, prop.nitems, prop.value);
    }
}

#define print_props(owin) do { \
    print_prop(owin, XA_WM_PROTOCOLS); \
    print_prop(owin, XA_WM_DELETE_WINDOW); \
    print_prop(owin, XA_WM_LOCALE_NAME); \
    print_prop(owin, XA_WM_COMMAND); \
    print_prop(owin, XA_WM_CLASS); \
    print_prop(owin, XA_WM_NAME); \
    print_prop(owin, XA_WM_WINDOW_ROLE); \
    print_prop(owin, XA_WM_CLIENT_MACHINE); \
    print_prop(owin, XA_CLIPBOARD); \
    print_prop(owin, XA_PRIMARY); \
    print_prop(owin, XA_SECONDARY); \
} while (0)

static void RedrawWindow(Display *display, Window win, int count = 0)
{
    XExposeEvent ev = { Expose, 0, True, display, win, 0, 0, FB_WIDTH, FB_HEIGHT, count };
    XSendEvent(display, win, False, 0 /*event_mask*/, (XEvent *) &ev);
    XFlush(display);
}

// TIMER

static Display *timer_dpy = NULL;
static Window timer_win = 0;
static Atom timer_atom = 0;
static struct sigaction timer_sa = {};
static struct timeval timer_last_tv = {};

static int timediff_ms(struct timeval *before, struct timeval *after)
{
    return (
        (long long) after->tv_sec * 1000000ll -
        (long long) before->tv_sec * 1000000ll +
        (long long) after->tv_usec -
        (long long) before->tv_usec
    ) / 1000;
}

static void timer_alarm(int/*sig*/, siginfo_t */*si*/, void */*ucontext*/)
{
    //ucontext_t *ctx = (ucontext_t *) ucontext;
    struct timeval now;
    gettimeofday(&now, NULL);
    long ms = timediff_ms(&timer_last_tv, &now);
    timer_last_tv = now;
    (void) ms;

    if (!timer_dpy || !timer_win) return;
    //XClientMessageEvent ev = { ClientMessage, 0, True, timer_dpy, timer_win, timer_atom, 32, {.l = {ms}} };
    //XSendEvent(timer_dpy, timer_win, False, 0 /*event_mask*/, (XEvent *) &ev);
    // process the framebuffer refresh timer (vsync-like)
    RedrawWindow(timer_dpy, timer_win, timer_atom);
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

int wait_for_vsync(int drmfd)
{
    drm_wait_vblank_t u = {};
    u.request.type = _DRM_VBLANK_RELATIVE;
    u.request.sequence = 1;
    return ioctl(drmfd, DRM_IOCTL_WAIT_VBLANK, &u);
}

void swap_buffers(Display *display, Window win, XdbeBackBuffer dbe)
{
    if (dbe) {
        XdbeSwapInfo si = { .swap_window = win, .swap_action = XdbeUndefined };
        XdbeSwapBuffers(display, &si, 1);
    }
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
    int drmfd = open("/dev/dri/card0", O_RDWR | O_CLOEXEC);
    if (drmfd == -1) perror("open /dev/dri/card0"), exit(1);
    printf("drmfd = %d\n", drmfd);

#if 0
    // retrieve resources
    drmModeRes *res = drmModeGetResources(drmfd);
    if (!res) {
        fprintf(stderr, "DRM: cannot retrieve DRM resources (%d): %m\n", errno);
        return -errno;
    }

    // iterate all connectors
    for (int i = 0; i < res->count_connectors; i++) {
        // get information for each connector
        drmModeConnector *conn = drmModeGetConnector(drmfd, res->connectors[i]);
        if (!conn) {
            fprintf(stderr, "DRM: cannot retrieve DRM connector %u:%u (%d): %m\n", i, res->connectors[i], errno);
            continue;
        }

        // check if a monitor is connected
        if (conn->connection != DRM_MODE_CONNECTED) {
            fprintf(stderr, "DRM: ignoring unused connector %u\n", conn->connector_id);
            return -ENOENT;
        }

        // check if there is at least one valid mode
        if (conn->count_modes == 0) {
            fprintf(stderr, "DRM: no valid mode for connector %u\n", conn->connector_id);
            return -EFAULT;
        }

        // print the mode information
        fprintf(stderr, "DRM: mode for connector %u is %ux%u\n",
            conn->connector_id, conn->modes[0].hdisplay, conn->modes[0].vdisplay);

        // free connector data
        drmModeFreeConnector(conn);
    }
#endif

    int pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) perror("sysconf"), exit(1);

    char *locale = setlocale(LC_ALL, "");
    if (locale == NULL) perror("setlocale"), exit(1);

    printf("pagesize = %d\n", pagesize); // 4096
    printf("locale = %s\n", locale);
    printf("XInitThreads() = %d\n", XInitThreads()); // 1
    printf("XOpenDisplay = %p\n", display = XOpenDisplay(NULL));

    debX11init(display);

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

    printf("Visual %p ext_data=%p visualid=%lu class=%s "
        "rm=%lx gm=%lx bm=%lx bits_per_rgb=%d map_entries=%d\n", visual,
        /* XExtData* */     visual->ext_data,               // hook for extension to hang data
        /* VisualID */      visual->visualid,               // visual id of this visual
        /* int */           debX11_vclass(visual->c_class), // C++ class of screen (monochrome, etc.)
        /* unsigned long */ visual->red_mask, visual->green_mask, visual->blue_mask,
        /* int */           visual->bits_per_rgb,           // log base 2 of distinct color values
        /* int */           visual->map_entries             // color map entries
    );

    // Show a visuals
    if (0) {
        int vlist_cnt = 0;
        XVisualInfo vinfo, *vlist;
        vinfo.screen = screen;
        vlist = XGetVisualInfo(display, VisualScreenMask, &vinfo, &vlist_cnt);
        if (!vlist) fprintf(stderr, "No visual info for screen %d\n", screen);
        for (int i = 0; i < vlist_cnt; i++) {
            printf("Visual[%d] %p id=%lu screen=%d depth=%d class=%s "
                "rm=%lx gm=%lx bm=%lx colormap_size=%d bits_per_rgb=%d\n", i,
                /* Visual* */       vlist[i].visual,
                /* VisualID (ul) */ vlist[i].visualid,
                /* int */           vlist[i].screen,
                /* int */           vlist[i].depth,
                /* int */           debX11_vclass(vlist[i].c_class),
                /* unsigned long */ vlist[i].red_mask,
                /* unsigned long */ vlist[i].green_mask,
                /* unsigned long */ vlist[i].blue_mask,
                /* int */           vlist[i].colormap_size,
                /* int */           vlist[i].bits_per_rgb
            );
        }
        printf("\n");
        XFree(vlist);
    }

    int pflist_count;
    XPixmapFormatValues *pflist = XListPixmapFormats(display, &pflist_count);
    for (int i = 0; i < pflist_count; i++)
        printf("PixmapFormat[%d] depth=%d bpp=%d scanline_pad=%d\n", i,
            pflist[i].depth, pflist[i].bits_per_pixel, pflist[i].scanline_pad);
    printf("\n");
    XFree(pflist);

    /* ATOMS */

    DefineAtom(clip);
    DefineAtom(NULL);
    //DefineAtom(ATOM);
    DefineAtom(TARGETS);
    DefineAtom(UTF8_STRING);
    DefineAtom(COMPOUND_TEXT);
    DefineAtom(TEXT);
    //DefineAtom(STRING);
    DefineAtom(MULTIPLE);
    DefineAtomValue(TEXT_PLAIN, "text/plain");
    DefineAtomValue(TEXT_HTML, "text/html");

    DefineAtom(WM_PROTOCOLS);
    DefineAtom(WM_DELETE_WINDOW);
    DefineAtom(WM_LOCALE_NAME);
    //DefineAtom(WM_COMMAND); /* defined in /usr/include/X11/Xatom.h */
    //DefineAtom(WM_CLASS);
    //DefineAtom(WM_NAME);
    DefineAtom(WM_WINDOW_ROLE);
    //DefineAtom(WM_CLIENT_MACHINE);

    //DefineAtom(PRIMARY);
    //DefineAtom(SECONDARY);
    DefineAtom(CLIPBOARD);
    DefineAtom(_NET_WM_PING);
    DefineAtom(_MOTIF_WM_HINTS);

    /* WINDOW PART */

    XSetWindowAttributes wattr = {};
    wattr.background_pixel = XBlackPixel(display, XDefaultScreen(display));
    wattr.bit_gravity = NorthWestGravity;
    wattr.win_gravity = NorthWestGravity;
    wattr.backing_store = NotUseful; // NotUseful, WhenMapped, Always
    //wattr.backing_store = Always;
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
    //XSetWMHints(display, win, &wm_hints);

    XClassHint class_hints = {};
    class_hints.res_name = (char *) THIS_RESNAME;
    class_hints.res_class = (char *) THIS_CLASSNAME;
    //XSetClassHint(display, win, &class_hints);

    unsigned char name_uc[] = THIS_TITLE;
    XTextProperty name_prop = { name_uc, XA_STRING, 8, strlen((char *) name_uc) };
    XSetWMProperties(display, win, &name_prop, &name_prop, argv, argc, &size_hints, &wm_hints, &class_hints);

    Atom protocols[] = { XA_WM_DELETE_WINDOW, XA__NET_WM_PING, };
    XSetWMProtocols(display, win, protocols, ARRAY_SIZE(protocols));

    // _MOTIF_WM_HINTS(_MOTIF_WM_HINTS) = 0x3, 0x2c, 0x3a, 0x0, 0x0
    typedef struct {
        unsigned long flags;
        unsigned long functions;
        unsigned long decorations;
        long input_mode;
        unsigned long status;
    } MwmHints;
    enum {
        MWM_HINTS_FUNCTIONS =	(1L << 0),
        MWM_HINTS_DECORATIONS =	(1L << 1),
        MWM_FUNC_ALL =		(1L << 0),
        MWM_FUNC_RESIZE =	(1L << 1),
        MWM_FUNC_MOVE =		(1L << 2),
        MWM_FUNC_MINIMIZE =	(1L << 3),
        MWM_FUNC_MAXIMIZE =	(1L << 4),
        MWM_FUNC_CLOSE =	(1L << 5),
        MWM_DECOR_ALL =		(1L << 0),
        MWM_DECOR_BORDER =	(1L << 1),
        MWM_DECOR_RESIZEH =	(1L << 2),
        MWM_DECOR_TITLE =	(1L << 3),
        MWM_DECOR_MENU =	(1L << 4),
        MWM_DECOR_MINIMIZE =	(1L << 5),
        MWM_DECOR_MAXIMIZE =	(1L << 6),
        MWM_INPUT_MODELESS = 0,
        MWM_INPUT_PRIMARY_APPLICATION_MODAL = 1,
        MWM_INPUT_SYSTEM_MODAL = 2,
        MWM_INPUT_FULL_APPLICATION_MODAL = 3,
        MWM_INPUT_APPLICATION_MODAL = MWM_INPUT_PRIMARY_APPLICATION_MODAL,
        MWM_TEAROFF_WINDOW =	(1L<<0)
    };
    MwmHints mwmhints;
    mwmhints.flags = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
    mwmhints.functions = MWM_FUNC_RESIZE | MWM_FUNC_MINIMIZE | MWM_FUNC_MAXIMIZE | MWM_FUNC_CLOSE;
    mwmhints.decorations = MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MENU | MWM_DECOR_MINIMIZE;
    XChangeProperty(display, win, XA__MOTIF_WM_HINTS, XA__MOTIF_WM_HINTS, 32, PropModeReplace, (unsigned char*)&mwmhints, 5);

    // Clear backgroud, it looks more pleasant before the redraw event does its job
    XSetWindowBackground(display, win, 0x333333); // dark gray

    // We want events associated with the specified event mask
    unsigned long mask =
        KeyPressMask | KeyReleaseMask |
        ButtonPressMask | ButtonReleaseMask |
        EnterWindowMask | LeaveWindowMask |
        PointerMotionMask |
        //PointerMotionHintMask | // send only one MotionNotify event (with the is_hint member of the XPointerMovedEvent structure set to NotifyHint)
        // until either the key or button state changes, the pointer leaves the event window, or the client calls XQueryPointer() or XGetMotionEvents()
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
    print_props(win);

    /* Show display's bytes */
    if (1) {
        int len;
        char *data = XFetchBytes(display, &len);
        debX11str(win, "XFetchBytes: len=%d data='%s'", len, data);
        XFree(data);
    }

    /* Configure input */

    XIMStyles *xim_styles;
    XIMStyle input_style;
    char *newlocale = NULL;

    // Locale setting taken from XtSetLanguageProc
    if ((newlocale = setlocale(LC_ALL, "")) == NULL) {
        fprintf(stderr, "I18N warning: Locale not supported by C library\n");
    }

    if (!XSupportsLocale()) {
        fprintf(stderr, "I18N warning: Locale not supported by Xlib\n");
        newlocale = setlocale(LC_ALL, "C");
    }

    char *modifiers = NULL;

    // try with XMODIFIERS env. var.
    if (xim == NULL) {
        if ((modifiers = XSetLocaleModifiers("")) != NULL && *modifiers) {
            xim = XOpenIM(display, NULL, NULL, NULL);
        } else {
            fprintf(stderr, "I18N warning: $XMODIFIERS not supported\n");
        }
    }

    // try with no modifiers base
    if (xim == NULL) {
        if ((modifiers = XSetLocaleModifiers("@im=none")) != NULL && *modifiers) {
            xim = XOpenIM(display, NULL, NULL, NULL);
        } else {
            fprintf(stderr, "I18N warning: @im=none modifier not supported\n");
        }
    }

    if (xim == NULL) {
        // are there languages without Input Methods?
        fprintf(stderr, "I18N error: Input method not specified\n");
        return 1;
    }

    fprintf(stderr, "XLocaleOfIM = %s\n", XLocaleOfIM(xim));

    if (XGetIMValues(xim, XNQueryInputStyle, &xim_styles, NULL) || xim_styles == NULL) {
        fprintf(stderr, "I18N error: Input method doesn't support any style\n");
        return 1;
    }
    for (unsigned i = 0; i < xim_styles->count_styles; i++) {
        fprintf(stderr, "xim_styles[%d] = %#010lx\n", i, xim_styles->supported_styles[i]);
    }
    XFree(xim_styles);

    if (newlocale != NULL && (strstr(newlocale, XLocaleOfIM(xim)) == NULL)) {
        fprintf(stderr, "I18N warning: system locale: '%s' differs from IM locale: '%s'\n", newlocale, XLocaleOfIM(xim));
    }

    input_style = XIMPreeditNothing | XIMStatusNothing;
    xic = XCreateIC(xim, XNInputStyle, input_style, XNClientWindow, win, XNFocusWindow, win, NULL);
    if (xic == NULL) {
        fprintf(stderr, "I18N error: Failed to create input context\n");
        return 1;
    }

    unsigned long icmask = 0;
    if (XGetICValues(xic, XNFilterEvents, &icmask, NULL)) {
        fprintf(stderr, "I18N error: Can't get Event Mask\n");
    }
    fprintf(stderr, "XGetICValues icmask=%lx\n", icmask);

    /* Show clipboard/selection */

    if (0) {
        // Show selections
        Atom selections[] = { XA_PRIMARY,  XA_SECONDARY, XA_CLIPBOARD };
        for (size_t seli = 0; seli < ARRAY_SIZE(selections); seli++) {
            Atom selection = selections[seli];
            Window owin = XGetSelectionOwner(display, selection);
            debX11str(win, "\n\n=== XGetSelectionOwner(%T) = %T", debX11_atom(selection), debX11_win(owin));

            if (owin != None) {
                print_props(owin);

                Atom req_types[] = {
                    XA_TARGETS,
                    XA_UTF8_STRING,   // Doesn't work in rxvt(FAIL)
                    XA_COMPOUND_TEXT, // Doesn't work in Opera("@8@>20!",lost=0), Chrome("    ",lost=ALL), FF(prop=None)
                    XA_TEXT,          // Doesn't work in Opera(FAIL), Chrome(FAIL), FF("\u0440\u0438\u0...",lost=0)
                    XA_STRING,        // Doesn't work in Opera("@8@>20!",lost=0), Chrome("<UTF-8>",lost=0), FF("\u0440\u0438\u0...",lost=0)
                    XA_MULTIPLE,
                    XA_TEXT_PLAIN,
                    XA_TEXT_HTML,
                };

                for (size_t i = 0; i < ARRAY_SIZE(req_types); i++) {
                    Atom req_type = req_types[i];
                    time_t time_started;
                    XEvent event;
                    XTextProperty prop;

                    XConvertSelection(display, selection, req_type, XA_clip, win, CurrentTime);

                    time_started = time(NULL);

                    while (!XCheckTypedWindowEvent(display, win, SelectionNotify, &event)) {
                        time_t tnow = time(NULL);
                        if (time_started > tnow)
                            time_started = tnow;
                        if (tnow - time_started >= 5) {
                            time_started = 0;
                            break;
                        }
                        usleep(1000);
                    }
                    if (time_started == 0) {
                        debX11ev(win, &event, "Wait for SelectionNotify failed!");
                        continue;
                    }
                    if (event.type != SelectionNotify) {
                        debX11ev(win, &event, "Got not a SelectionNotify!");
                        continue;
                    }
                    // XSelectionEvent
                    if (event.xselection.property == None) {
                        debX11ev(win, &event, "target=%s prop=%s ",
                                 debX11_atom(event.xselection.target),
                                 debX11_atom(event.xselection.property));
                        continue;
                    }

                    if (!XGetTextProperty(display, event.xselection.requestor,
                                          &prop, event.xselection.property)) {
                        debX11ev(win, &event, "XGetTextProperty failed!");
                        continue;
                    }
                    /*
                     XGetWindowProperty(display, win, Atom property, long
                          offset, long length, Bool delete, Atom req_type, Atom
                          *actual_type, int *actual_format, unsigned long
                          *nitems, unsigned long *bytes_after, unsigned char
                          **prop)
                    */

                    std::string value;

                    if (event.xselection.target == XA_TARGETS) {
                        Atom *a = (Atom *) prop.value;
                        for (unsigned i = 0; i < prop.nitems; i++)
                            value += ssprintf("%s%T", value.size() ? " " : "", debX11_atom(a[i]));
                        prop.value = (unsigned char *) value.c_str();
                    }

                    debX11ev(win, &event, "target=%s prop=%s "
                             "encoding=%s format=%d [%lu]: '%s'",
                             debX11_atom(event.xselection.target),
                             debX11_atom(event.xselection.property),
                             debX11_atom(prop.encoding), prop.format,
                             prop.nitems, prop.value);
                }
            }
        }
    }

    /* MAIN */

    // https://www.x.org/releases/current/doc/libXext/dbelib.html
    // https://www.x.org/releases/current/doc/xextproto/dbe.html
    XdbeBackBuffer dbe = None;
    if (use_dbe) {
        int dbe_ext = 0, dbe_major = 0, dbe_minor = 0;
        dbe_ext = XdbeQueryExtension(display, &dbe_major, &dbe_minor);
        if (dbe_ext) {
            dbe = XdbeAllocateBackBufferName(display, win, XdbeCopied);
            if (!dbe) dbe_ext = 0;
        }
        printf("XdbeQueryExtension() v%d.%d dbe_ext=%d dbe=%lx\n", dbe_major, dbe_minor, dbe_ext, dbe);
    }

    XFontStruct *fnt = XLoadQueryFont(display, "6x10");

    XGCValues gcv = {};
    gcv.font = fnt->fid;
    gcv.foreground = XWhitePixel(display, screen);
    GC gc = XCreateGC(display, win, GCFont | GCForeground, &gcv);
    //Pixmap framebuffer attr_cleanup_pixmap = XCreatePixmap(display, win, FB_WIDTH, FB_HEIGHT, 24);

    // https://www.x.org/releases/current/doc/xextproto/shm.html
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
        //shmimg = XGetImage(display, rootwin, 0, 0, FB_WIDTH, FB_HEIGHT, 0, ZPixmap);
        shmimg = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, FB_WIDTH, FB_HEIGHT, bitmap_pad, 0);
        if (shmimg) {
            shmimg->data = (char *) aligned_alloc(pagesize, shmimg->bytes_per_line * shmimg->height);
            if (!shmimg->data) { XDestroyImage(shmimg); shmimg = NULL; }
        }
        shared_pixmaps = 0;
    }
    printf("XShmQueryVersion() v%d.%d shared_pixmaps=%d shmimg=%p\n",
        mitshm_major, mitshm_minor, shared_pixmaps, shmimg);

    if (shmimg) {
        printf("XShmCreateImage() %dx%d @%d format=%s data=%p byte_order=%d bitmap_unit=%d "
            "bitmap_bit_order=%d bitmap_pad=%d depth=%d bytes_per_line=%d "
            "bits_per_pixel=%d rm=%lx gm=%lx bm=%lx obdata=%p\n",
            shmimg->width, shmimg->height, shmimg->xoffset, // number of pixels offset in X direction
            debX11_image_format(shmimg->format), // XYBitmap, XYPixmap, ZPixmap
            shmimg->data,                  // (char *) pointer to image data
            shmimg->byte_order,            // data byte order, LSBFirst, MSBFirst
            shmimg->bitmap_unit,           // quant. of scanline 8, 16, 32
            shmimg->bitmap_bit_order,      // LSBFirst, MSBFirst
            shmimg->bitmap_pad,            // 8, 16, 32 either XY or ZPixmap
            shmimg->depth,                 // depth of image
            shmimg->bytes_per_line,        // accelerator to next scanline
            shmimg->bits_per_pixel,        // bits per pixel (ZPixmap)
            shmimg->red_mask,              // bits in z arrangement
            shmimg->green_mask,
            shmimg->blue_mask,
            shmimg->obdata                 // hook for the object routines to hang on
        );
        if (0)
        if (shmimg->data)
        for (int y = 0; y < shmimg->height; y += 4) {
            unsigned *p = (unsigned *) (shmimg->data + shmimg->bytes_per_line * (y+0));
            unsigned *q = (unsigned *) (shmimg->data + shmimg->bytes_per_line * (y+1));
            unsigned *r = (unsigned *) (shmimg->data + shmimg->bytes_per_line * (y+2));
            unsigned *s = (unsigned *) (shmimg->data + shmimg->bytes_per_line * (y+3));
            printf("|");
            for (int x = 0; x < shmimg->width; x += 4)
                printf("%c", p[x] || p[x+1] || p[x+2] || p[x+3] ||
                             q[x] || q[x+1] || q[x+2] || q[x+3] ||
                             r[x] || r[x+1] || r[x+2] || r[x+3] ||
                             s[x] || s[x+1] || s[x+2] || s[x+3] ? '@' : ' ');
            printf("|\n");
        }
    }

    /*
    //XImage *memimg = XGetImage(display, rootwin, 0, 0, FB_WIDTH, FB_HEIGHT, 0, ZPixmap);
    XImage *memimg = XCreateImage(display, visual, depth, ZPixmap, 0, NULL, FB_WIDTH, FB_HEIGHT, bitmap_pad, 0);
    if (memimg) {
        memimg->data = (char *) aligned_alloc(pagesize, memimg->bytes_per_line * memimg->height);
        if (!memimg->data) { XDestroyImage(memimg); memimg = NULL; }
    }
    if (memimg) {
        printf("XCreateImage()    %dx%d @%d format=%s data=%p byte_order=%d bitmap_unit=%d "
            "bitmap_bit_order=%d bitmap_pad=%d depth=%d bytes_per_line=%d "
            "bits_per_pixel=%d rm=%lx gm=%lx bm=%lx obdata=%p\n",
            memimg->width, memimg->height, memimg->xoffset, // number of pixels offset in X direction
            debX11_image_format(memimg->format), // XYBitmap, XYPixmap, ZPixmap
            memimg->data,                  // (char *) pointer to image data
            memimg->byte_order,            // data byte order, LSBFirst, MSBFirst
            memimg->bitmap_unit,           // quant. of scanline 8, 16, 32
            memimg->bitmap_bit_order,      // LSBFirst, MSBFirst
            memimg->bitmap_pad,            // 8, 16, 32 either XY or ZPixmap
            memimg->depth,                 // depth of image
            memimg->bytes_per_line,        // accelerator to next scanline
            memimg->bits_per_pixel,        // bits per pixel (ZPixmap)
            memimg->red_mask,              // bits in z arrangement
            memimg->green_mask,
            memimg->blue_mask,
            memimg->obdata                 // hook for the object routines to hang on
        );
        if (1)
        if (memimg->data)
        for (int y = 0; y < memimg->height; y += 4) {
            unsigned *p = (unsigned *) (memimg->data + memimg->bytes_per_line * (y+0));
            unsigned *q = (unsigned *) (memimg->data + memimg->bytes_per_line * (y+1));
            unsigned *r = (unsigned *) (memimg->data + memimg->bytes_per_line * (y+2));
            unsigned *s = (unsigned *) (memimg->data + memimg->bytes_per_line * (y+3));
            printf("|");
            for (int x = 0; x < memimg->width; x += 4)
                printf("%c", p[x] || p[x+1] || p[x+2] || p[x+3] ||
                             q[x] || q[x+1] || q[x+2] || q[x+3] ||
                             r[x] || r[x+1] || r[x+2] || r[x+3] ||
                             s[x] || s[x+1] || s[x+2] || s[x+3] ? '@' : ' ');
            printf("|\n");
        }
    }
    // XShmCreateImage() 800x600 @0 format=ZPixmap data=0xb748d000 byte_order=0 bitmap_unit=32 bitmap_bit_order=0 bitmap_pad=32 depth=24 bytes_per_line=3200 bits_per_pixel=32 rm=ff0000 gm=ff00 bm=ff obdata=0xbfb5c2cc
    // XCreateImage()    800x600 @0 format=ZPixmap data=0xb72b8000 byte_order=0 bitmap_unit=32 bitmap_bit_order=0 bitmap_pad=32 depth=24 bytes_per_line=3200 bits_per_pixel=32 rm=ff0000 gm=ff00 bm=ff obdata=(nil)
    */
    //getchar(); exit(1);

    if (shmimg)
        pframebuf = (framebuf_t *) shmimg->data;

    // Show the window
    //XResizeWindow(display, win, ScreenCols * FontCX, ScreenRows * FontCY);
    XMapRaised(display, win); // -> Expose event
    XSync(display, false);

    // Wait for the MapNotify event
    if (0) for (;;) {
        XEvent event; XNextEvent(display, &event);
        debX11ev(win, &event, "");
        if (event.type == MapNotify) break;
    }

    void *ptr = mmap((void*)0x20000000, 4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED|MAP_LOCKED|MAP_POPULATE, -1, 0);
    printf("=== mmap=%p\n", ptr);
    munmap(ptr, 4096);

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
    const char *msg = "Hello, World!";

    while (!done) {
        XEvent ev;
        XNextEvent(display, &ev);
        switch (ev.type) {
            case Expose: {
                XExposeEvent *xev = &ev.xexpose;

                static int prevcount, cnt = 1;
                if (0 && prevcount != xev->count) debX11ev(win, &ev, "[%d] serial=%lu xy=%d,%d wh=%d,%d count=%d", cnt++,
                    xev->serial, xev->x, xev->y, xev->width, xev->height, xev->count);
                prevcount = xev->count;

                if (xev->count != (int)XA_NULL || memcmp(&oldframebuf, pframebuf, sizeof(oldframebuf)) != 0) {
                    Drawable w = dbe ? dbe : win;
                    if (shared_pixmaps) {
                        if (dbe) {
                            XShmPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT, 1/*send_event*/);
                        } else {
                            wait_for_vsync(drmfd);
                            XShmPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT, 0/*send_event*/);
                        }
                    } else {
                        if (dbe) {
                            XPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT);
                            XFlush(display);
                            wait_for_vsync(drmfd);
                            swap_buffers(display, win, dbe);
                        } else {
                            wait_for_vsync(drmfd);
                            XPutImage(display, w, gc, shmimg, 0, 0 /*src*/, 0, 0 /*dest*/, FB_WIDTH, FB_HEIGHT);
                            XFlush(display);
                        }
                    }
                    if (1) {
                        XSetForeground(display, gc, XWhitePixel(display, screen));
                        XFillRectangle(display, w, gc, 20, 20, 10, 10);
                        XDrawString(display, w, gc, 10, 50, msg, strlen(msg));
                        // Draw the line
                        //GC gc = XCreateGC(display, win, 0, NULL);
                        //XSetForeground(display, gc, WhitePixel(display, screen));
                        XSetForeground(display, gc, 0xff1133);
                        XDrawLine(display, w, gc, 40, 20, 90, 30);
                        //XFlush(display);
                    }
                    memcpy(&oldframebuf, pframebuf, sizeof(oldframebuf));
                }
                XEvent evtmp; // drain Expose messages
                while (XCheckTypedEvent(display, Expose, &evtmp));
                if (getkey_q.size() <= 1) { getkey_q.put(Key::NextFrame); nredraws++; }
                break;
            }
            case KeyPress:
            case KeyRelease: {
                static int cnt = 1;

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

                if (1) {
                    char keys[32] = {};
                    int resqk = XQueryKeymap(display, keys);
                    printf("XQueryKeymap() = %d:", resqk);
                    for (size_t i = 0; i < sizeof(keys); i++) printf(" %02x", keys[i]);
                    printf("\n");
                }

                if (nbytes == 1 && str[0] < ' ') {
                    str[1] = str[0] + '@';
                    str[0] = '^';
                    str[2] = '\0';
                    nbytes = 2;
                }

                int xkey = key & 511;
                XKeyEvent *xev = &ev.xkey;
                debX11ev2(win, &ev, "<-", xev->root, "[%d] %T", cnt++,
                    ssprintf("subwindow=%T time=%lu xy=%d,%d xy_root=%d,%d state=%x keycode=%u keysym=%lx ks=%s(%lx) ksxkb=%s str='%.*s'(%db) key='%s%c' same_screen=%d",
                        //*Window*/ debX11_win(xev->root),	/* root window that the event occurred on */
                        /*Window*/ debX11_win(xev->subwindow),	/* child window */
                        /*Time*/ xev->time,			/* milliseconds */
                        /*int*/ xev->x, xev->y,			/* pointer x, y coordinates in event window */
                        /*int*/ xev->x_root, xev->y_root,	/* coordinates relative to root */
                        /*unsigned int*/ xev->state,		/* key or button mask */
                        /*unsigned int*/ xev->keycode, keysym, XKeysymToString(XLookupKeysym(&ev.xkey, 0)), XLookupKeysym(&ev.xkey, 0),
                        XKeysymToString(XkbKeycodeToKeysym(display, xev->keycode, 0, 0)), nbytes, str, nbytes, xkey < 32 ? "^" : "", xkey < 32 ? xkey + '@' : xkey,
                        /*Bool*/ xev->same_screen       	/* same screen flag */
                    )
                );

                done = key == 'q';
                break;
            }
            case MotionNotify: {
                static int cnt = 1;
                XMotionEvent *xev = &ev.xmotion;
                if (0)
                debX11ev2(win, &ev, "<-", xev->root, "[%d] %T", cnt++,
                    ssprintf("subwindow=%T time=%lu xy=%d,%d xy_root=%d,%d state=%x is_hint=%d same_screen=%d",
                        //*Window*/ debX11_win(xev->root),	/* root window that the event occurred on */
                        /*Window*/ debX11_win(xev->subwindow),	/* child window */
                        /*Time*/ xev->time,      		/* milliseconds */
                        /*int*/ xev->x, xev->y,       		/* pointer x, y coordinates in event window */
                        /*int*/ xev->x_root, xev->y_root,	/* coordinates relative to root */
                        /*unsigned int*/ xev->state,     	/* key or button mask */
                        /*char*/ xev->is_hint,
                        /*Bool*/ xev->same_screen       	/* same screen flag */
                    )
                );
                int key = Key::MouseMove | (xev->x & 2047) << 9 | (xev->y & 2047) << 20;
                getkey_q.put(key);
                break;
            }
            case ButtonPress:
            case ButtonRelease: {
                static int cnt = 1;
                XButtonEvent *xev = &ev.xbutton;
                if (0)
                debX11ev2(win, &ev, "<-", xev->root, "[%d] %T", cnt++,
                    ssprintf("subwindow=%T time=%lu xy=%d,%d xy_root=%d,%d state=%x button=%u same_screen=%d",
                        //*Window*/ debX11_win(xev->root),	/* root window that the event occurred on */
                        /*Window*/ debX11_win(xev->subwindow),	/* child window */
                        /*Time*/ xev->time,      		/* milliseconds */
                        /*int*/ xev->x, xev->y,       		/* pointer x, y coordinates in event window */
                        /*int*/ xev->x_root, xev->y_root,	/* coordinates relative to root */
                        /*unsigned int*/ xev->state,     	/* key or button mask */
                        /*unsigned int*/ xev->button,
                        /*Bool*/ xev->same_screen       	/* same screen flag */
                    )
                );
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
            case EnterNotify:
            case LeaveNotify: {
                static int cnt = 1;
                XCrossingEvent *xev = &ev.xcrossing;
                debX11ev2(win, &ev, "<-", xev->root, "[%d] %T", cnt++,
                    ssprintf("subwindow=%T time=%lu xy=%d,%d xy_root=%d,%d mode=%d detail=%d same_screen=%d focus=%d state=%x",
                        //*Window*/ debX11_win(xev->root),	/* root window that the event occurred on */
                        /*Window*/ debX11_win(xev->subwindow),	/* child window */
                        /*Time*/ xev->time,      		/* milliseconds */
                        /*int*/ xev->x, xev->y, 	      	/* pointer x, y coordinates in event window */
                        /*int*/ xev->x_root, xev->y_root,	/* coordinates relative to root */
                        /*int*/ xev->mode,     			/* NotifyNormal, NotifyGrab, NotifyUngrab */
                        /*int*/ xev->detail,			/* NotifyAncestor, NotifyVirtual, NotifyInferior, NotifyNonlinear,NotifyNonlinearVirtual */
                        /*Bool*/ xev->same_screen,		/* same screen flag */
                        /*Bool*/ xev->focus,			/* same screen flag */
                        /*unsigned int*/ xev->state     	/* key or button mask */
                    )
                );
                break;
            }
            case ClientMessage: {
                static int cnt = 1;
                XClientMessageEvent *xev = &ev.xclient;
                debX11ev2(win, &ev, "<-", xev->window, "[%d] message_type=%T format=%d serial=%ul", cnt++,
                        debX11_atom(xev->message_type), xev->format, xev->serial);
                // https://specifications.freedesktop.org/wm-spec/wm-spec-1.3.html
                if (xev->message_type == XA_WM_PROTOCOLS && xev->format == 32 && xev->data.l[0] == (long) XA__NET_WM_PING) {
                    xev->window = rootwin; // reply to Window Manager that we're alive
                    XSendEvent(display, xev->window, False, SubstructureNotifyMask | SubstructureRedirectMask, &ev);
                }
                break;
            }
            case GenericEvent: {
                static int cnt = 1;
                XGenericEventCookie *xev = &ev.xcookie;
                debX11ev(win, &ev, "[%d] serial=%ul display=%p extension=%x evtype=%d cookie=%x data=%p", cnt++,
                    xev->serial, xev->display, xev->extension, xev->evtype, xev->cookie, xev->data);
                break;
            }
            default: {
                if (ev.type == ShmCompletionEvent) {
                    if (0) {
                        XShmCompletionEvent *sce = (XShmCompletionEvent *) &ev;
                        debX11ev(win, &ev, "ShmReq=%d.%d shmseg=%x off=%ld",
                            sce->major_code, sce->minor_code, sce->shmseg, sce->offset);
                    }
                    if (dbe) {
                        wait_for_vsync(drmfd);
                        swap_buffers(display, win, dbe);
                    }
                } else {
                    debX11ev(win, &ev, "");
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

#include <dwmapi.h> // DwmFlush()

#define IDT_TIMER1 0x14001101
#define IDT_TIMER2 0x14001102

#define ID_ABOUT 2000
#define ID_EXIT 2001

#ifndef GET_X_LPARAM
#define GET_X_LPARAM(lp) ((int)(short)LOWORD(lp))
#define GET_Y_LPARAM(lp) ((int)(short)HIWORD(lp))
#endif

#include "debWin.cpp"

HWND gWnd = 0;
HANDLE gThread = 0;
static BOOL gModalState = false; // Is messagebox shown

static int method = 1; // 1 - CreateCompatibleBitmap/SetDIBits, 2 - CreateDIBSection, 3 - SetDIBitsToDevice
static int start = 20;

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
    if(!SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL)) {
        MessageBox(NULL, "Can't set thread priority!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
    }
    // Ask MMCSS to temporarily boost the thread priority
    DWORD taskIndex = 0;
    HANDLE hTask = AvSetMmThreadCharacteristics(TEXT("Games"), &taskIndex);
    if (hTask == NULL) {
        MessageBox(NULL, "Can't set MmThreadCharacteristics!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
    } else if (!AvSetMmThreadPriority(hTask, AVRT_PRIORITY_HIGH)) {
        MessageBox(NULL, "Can't set MmThreadPriority!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
    }
    //asm volatile ("call asm_main" ::: "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");
    asm_main_text();
    if (hTask) AvRevertMmThreadCharacteristics(hTask);
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
    static HDC hdcMem;
    static HBITMAP gBitmap;
    static UINT_PTR gTimer1;
    static RECT rcClient; // client area rectangle
    static bool is_moving = 0;

    if (debug) {
        if ((uMsg != WM_PAINT && uMsg != WM_TIMER && uMsg != WM_USER) || debug > 1) {
            DWORD insend = InSendMessageEx(NULL);
            if (debug) printf("WndProc start uMsg=%s (%u), wParam=%d, lParam=%lx insend=%lx%s%s%s%s%s\n",
                debWin_msg(uMsg), uMsg, wParam, lParam, insend,
                insend & ISMEX_SEND ? " send" : "",
                insend & ISMEX_REPLIED ? " replied" : "",
                insend & ISMEX_NOTIFY ? " notify" : "",
                insend & ISMEX_CALLBACK ? " callback" : "",
                insend & ~15 ? " UNKNOWN" : ""
            );
        }
    }

    /*
    Message-identifier values are used as follows:
    The system reserves message-identifier values in the range 0x0000 through 0x03FF (the value of WM_USER  – 1) for system-defined messages. Applications cannot use these values for private messages.
    Values in the range 0x0400 (the value of WM_USER) through 0x7FFF are available for message identifiers for private window classes.
    If your application is marked version 4.0, you can use message-identifier values in the range 0x8000 (WM_APP) through 0xBFFF for private messages.
    The system returns a message identifier in the range 0xC000 through 0xFFFF when an application calls the RegisterWindowMessage function to register a message. The message identifier returned by this function is guaranteed to be unique throughout the system. Use of this function prevents conflicts that can arise if other applications use the same message identifier for different purposes.
    */
    switch (uMsg) {
        /*
        vc@hp:~$ wine ./a.exe 2
        lpCmdLine=2
        WM_GETMINMAXINFO
        (hp 1280x800) +8,+8      ptMaxSize=1288,808 ptMaxPosition=-4,-4, ptMinTrackSize=112,27, ptMaxTrackSize=1292,812
        (dell 1366x768) +16,+16  ptMaxSize=1382,784 ptMaxPosition=-8,-8, ptMinTrackSize=132,38, ptMaxTrackSize=1378,780
        (deti 1920x1080) +16,+16 ptMaxSize=1936,1096 ptMaxPosition=-8,-8, ptMinTrackSize=136,39, ptMaxTrackSize=1940,1100
        WndProc uMsg=WM_NCCREATE (129), wParam=0, lParam=8321724
        WM_NCCALCSIZE wParam=0 rect=[0,0 - 808,627]
        WM_CREATE 808x627 at 0,0, hInstance=00400000 parent=00000000 name=Title, class=WinCanvas method=2
        WM_CREATE CreateDIBSection=00010040, pframebuf=007F0000
                  CreateDIBSection=9E0517F8, pframebuf=02850000 (windows)
        WndProc uMsg=WM_SHOWWINDOW (24), wParam=1, lParam=0
        WndProc uMsg=WM_WINDOWPOSCHANGING (70), wParam=0, lParam=8322064
        WndProc uMsg=WM_GETICON (127), wParam=2, lParam=0			(dell)
        WndProc uMsg=WM_GETICON (127), wParam=1, lParam=0			(hp)
        WndProc uMsg=WM_GETICON (127), wParam=0, lParam=0			(hp)
        WndProc uMsg=WM_QUERYNEWPALETTE (783), wParam=0, lParam=0		(hp)
        WndProc uMsg=WM_ACTIVATEAPP (28), wParam=1, lParam=0
        WndProc uMsg=WM_NCACTIVATE (134), wParam=1, lParam=0
        WndProc uMsg=WM_ACTIVATE (6), wParam=1, lParam=0
        WndProc uMsg=WM_IME_SETCONTEXT (641), wParam=1, lParam=-1073741809	(dell,deti)
        WndProc uMsg=WM_IME_NOTIFY (642), wParam=2, lParam=0			(dell)
        WndProc uMsg=WM_SETFOCUS (7), wParam=0, lParam=0
        WM_NCPAINT 0
        WM_ERASEBKGND 0 is_moving=0
        WndProc uMsg=WM_WINDOWPOSCHANGED (71), wParam=0, lParam=8322064
        WndProc uMsg=WM_SIZE (5), wParam=0, lParam=41091880
        WM_MOVE 0 is_moving=0
        WM_PAINT 0
        WM_PAINT ps.rcPaint=[4,23 - 804,623] GetClientRect=808x627
        WM_MOUSEACTIVATE top=00020042 hittest=2 mousemsg=513			(hp)

        WndProc uMsg=WM_GETICON (127), wParam=1, lParam=0
        WndProc uMsg=WM_GETICON (127), wParam=2, lParam=0
        WndProc uMsg=WM_GETICON (127), wParam=0, lParam=0
        === Message hwnd=000D1062 message=Unk(799) (799), wParam=1, lParam=0 time=111660898 pt=501,364
        WndProc uMsg=Unk(799) (799), wParam=1, lParam=0
        {	(dell)
        === Message hwnd=000D1062 message=Unk(49311) (49311), wParam=0, lParam=0 time=111660913 pt=501,364
        WndProc uMsg=Unk(49311) (49311), wParam=0, lParam=0
        === Message hwnd=00000000 message=Unk(49214) (49214), wParam=18, lParam=0 time=111660913 pt=501,364
        }
        { 	(deti)
        === Message hwnd=00B50A9A message=Unk(49379) (49379), wParam=0, lParam=0 time=644283531 pt=638,671
        WndProc uMsg=Unk(49379) (49379), wParam=0, lParam=0
        === Message hwnd=0032122E message=Unk(96) (96), wParam=5, lParam=4 time=644283531 pt=638,671
        WM_GETMINMAXINFO got ptMaxSize=1936,1096 ptMaxPosition=-8,-8, ptMinTrackSize=136,39, ptMaxTrackSize=1940,1100
        }

        WM_NCHITTEST 0 is_moving=0
        === Message hwnd=00B50A9A message=WM_NCMOUSEMOVE (160), wParam=2, lParam=43975294 time=644283546 pt=638,671

        {	(dell)
        === Message hwnd=000D106E message=WM_TIMER (275), wParam=5, lParam=0 time=111661022 pt=501,364
        === Message hwnd=000D106E message=Unk(49218) (49218), wParam=0, lParam=0 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49218) (49218), wParam=0, lParam=0 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49219) (49219), wParam=2, lParam=111661054 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49219) (49219), wParam=3, lParam=111661054 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49219) (49219), wParam=4, lParam=111661054 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49218) (49218), wParam=0, lParam=0 time=111661054 pt=501,364
        === Message hwnd=000D106E message=Unk(49218) (49218), wParam=0, lParam=0 time=111661116 pt=501,364
        === Message hwnd=001E1066 message=WM_TIMER (275), wParam=1, lParam=0 time=111661225 pt=501,364
        }
        { 	(deti)
        === Message hwnd=0032122E message=Unk(96) (96), wParam=6, lParam=0 time=644283546 pt=638,671
        === Message hwnd=0032122E message=Unk(96) (96), wParam=1, lParam=0 time=644283546 pt=638,671
        === Message hwnd=0032122E message=Unk(96) (96), wParam=1, lParam=0 time=644283546 pt=638,671
        === Message hwnd=0032122E message=Unk(96) (96), wParam=1, lParam=0 time=644283546 pt=638,671
        === Message hwnd=00000000 message=WM_TIMER (275), wParam=2785, lParam=1979683088 time=644283640 pt=638,671
        === Message hwnd=004013A0 message=WM_TIMER (275), wParam=1, lParam=0 time=644283843 pt=638,671
        }

        [...] exit:

        === Message hwnd=000D1062 hwnd=000710AC message=WM_TIMER (275), wParam=1, lParam=0 time=111839987 pt=891,173
        WndProc uMsg=WM_IME_SELECT (645), wParam=1, lParam=1549568			+hp
        === Message hwnd=000D10C8 message=WM_KEYDOWN (256), wParam=81, lParam=1048577 time=111679259 pt=336,496 time=111840221 pt=891,173
        WndProc uMsg=WM_KEYDOWN (256), wParam=81, lParam=1048577
        === Message hwnd=000D1062 hwnd=000D10C8 message=WM_CHAR (258), wParam=113, lParam=1048577 time=111679259 pt=336,496 time=111840221 pt=891,173
        WM_CHAR 'q'
        WndProc uMsg=Unk(144) (144), wParam=0, lParam=0
        WndProc uMsg=WM_WINDOWPOSCHANGING (70), wParam=0, lParam=2685900
        WndProc uMsg=WM_WINDOWPOSCHANGED (71), wParam=0, lParam=2685900
        WndProc uMsg=WM_NCACTIVATE (134), wParam=0, lParam=0
        WndProc uMsg=WM_ACTIVATE (6), wParam=0, lParam=0
        WndProc uMsg=WM_ACTIVATEAPP (28), wParam=0, lParam=3896 lParam=5656
        WndProc uMsg=WM_KILLFOCUS (8), wParam=0, lParam=0
        WndProc uMsg=WM_IME_SETCONTEXT (641), wParam=0, lParam=-1073741809		+dell
        WndProc uMsg=WM_IME_NOTIFY (642), wParam=1, lParam=0				+dell
        WM_DESTROY
        WndProc uMsg=WM_NCDESTROY (130), wParam=0, lParam=0
        */

        case WM_GETMINMAXINFO: {
            MINMAXINFO *mmi = (MINMAXINFO *) lParam;
            /*
            typedef struct tagMINMAXINFO {
              POINT ptReserved;
              POINT ptMaxSize;		// The maximized width and height of the window.
              POINT ptMaxPosition;	// The position of the left top corner of the maximized window.
              POINT ptMinTrackSize;	// The minimum tracking width and height of the window. This value can be obtained programmatically from the system metrics SM_CXMINTRACK and SM_CYMINTRACK (see the GetSystemMetrics function).
              POINT ptMaxTrackSize;	// The maximum tracking width and height of the window. This value is based on the size of the virtual screen and can be obtained programmatically from the system metrics SM_CXMAXTRACK and SM_CYMAXTRACK (see the GetSystemMetrics function).
            } MINMAXINFO, *PMINMAXINFO, *LPMINMAXINFO;
            */
            // ptMaxSize=1288,808 ptMaxPosition=-4,-4, ptMinTrackSize=112,27, ptMaxTrackSize=1292,812
            if (debug) printf("WM_GETMINMAXINFO got ptMaxSize=%ld,%ld ptMaxPosition=%ld,%ld, ptMinTrackSize=%ld,%ld, ptMaxTrackSize=%ld,%ld\n",
                mmi->ptMaxSize.x, mmi->ptMaxSize.y, mmi->ptMaxPosition.x, mmi->ptMaxPosition.y,
                mmi->ptMinTrackSize.x, mmi->ptMinTrackSize.y, mmi->ptMaxTrackSize.x, mmi->ptMaxTrackSize.y);

            mmi->ptMaxSize.x = FB_WIDTH; mmi->ptMaxSize.y = FB_HEIGHT;
            mmi->ptMinTrackSize.x = FB_WIDTH; mmi->ptMinTrackSize.y = FB_HEIGHT;
            mmi->ptMaxTrackSize.x = FB_WIDTH; mmi->ptMaxTrackSize.y = FB_HEIGHT;

            if (debug) printf("WM_GETMINMAXINFO set ptMaxSize=%ld,%ld ptMaxPosition=%ld,%ld, ptMinTrackSize=%ld,%ld, ptMaxTrackSize=%ld,%ld\n",
                mmi->ptMaxSize.x, mmi->ptMaxSize.y, mmi->ptMaxPosition.x, mmi->ptMaxPosition.y,
                mmi->ptMinTrackSize.x, mmi->ptMinTrackSize.y, mmi->ptMaxTrackSize.x, mmi->ptMaxTrackSize.y);

            return -1; // message ignored
            return 0; // message processed by application
        }

        case WM_NCCALCSIZE: {
            RECT *rect = (RECT *) lParam;
            if (debug) printf("WM_NCCALCSIZE wParam=%d rect=[%ld,%ld - %ld,%ld]\n", wParam, rect->left, rect->top, rect->right, rect->bottom);
            // On entry, the structure contains the proposed window rectangle for the window. On exit, the structure should contain the screen coordinates of the corresponding window client area.
            if (wParam) return 0;
            break;
        }

        case WM_MOUSEACTIVATE: {
            HWND top = (HWND) wParam;
            unsigned hittest = LOWORD(lParam);
            unsigned mousemsg = HIWORD(lParam);
            if (debug) printf("WM_MOUSEACTIVATE top=%p hittest=%u mousemsg=%u\n", top, hittest, mousemsg);
            /*
            Parameters
            wParam A handle to the top-level parent window of the window being activated.
            lParam
                The low-order word specifies the hit-test value returned by the DefWindowProc function as a result of processing the WM_NCHITTEST message. For a list of hit-test values, see WM_NCHITTEST.
                The high-order word specifies the identifier of the mouse message generated when the user pressed a mouse button. The mouse message is either discarded or posted to the window, depending on the return value.
            Return value
            The return value specifies whether the window should be activated and whether the identifier of the mouse message should be discarded. It must be one of the following values.
            MA_ACTIVATE 1 Activates the window, and does not discard the mouse message.
            MA_ACTIVATEANDEAT 2 Activates the window, and discards the mouse message.
            MA_NOACTIVATE 3 Does not activate the window, and does not discard the mouse message.
            MA_NOACTIVATEANDEAT 4 Does not activate the window, but discards the mouse message.
            */
            InvalidateRect(top, NULL, false);
            return MA_ACTIVATEANDEAT;
        }

        case WM_CREATE: {
            CREATESTRUCT *cs = (CREATESTRUCT *) lParam;

            if (debug) printf("WM_CREATE %dx%d at %d,%d, hInstance=%p parent=%p name=%s, class=%s method=%d\n",
                cs->cx, cs->cy, cs->x, cs->y, cs->hInstance, cs->hwndParent, cs->lpszName, cs->lpszClass, method);

            if (method == 1) {
                pframebuf = &framebuf;
                HDC hdc = GetDC(hWnd);
                hdcMem = CreateCompatibleDC(hdc); // Temp HDC to copy picture
                gBitmap = CreateCompatibleBitmap(hdc, FB_WIDTH, FB_HEIGHT);
                if (debug) printf("WM_CREATE CreateCompatibleBitmap=%p, pframebuf=%p\n", gBitmap, pframebuf);
                HGDIOBJ old = SelectObject(hdcMem, gBitmap);
                DeleteObject(old); // release the 1x1@1bpp default bitmap
                ReleaseDC(hWnd, hdc);

            } else if (method == 2) {
                BITMAPINFO bmi = {};
                bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
                bmi.bmiHeader.biWidth = FB_WIDTH;
                bmi.bmiHeader.biHeight = -FB_HEIGHT; // top-down
                bmi.bmiHeader.biPlanes = 1;
                bmi.bmiHeader.biBitCount = 8*4;
                bmi.bmiHeader.biCompression = BI_RGB;
                bmi.bmiHeader.biSizeImage = ((((FB_WIDTH * bmi.bmiHeader.biBitCount) + 31) & ~31) >> 3) * FB_HEIGHT;

                HDC hdc = GetDC(hWnd);
                hdcMem = CreateCompatibleDC(hdc); // Temp HDC to copy picture
                gBitmap = CreateDIBSection(
                    hdc,		// hdc A handle to a device context
                    &bmi,		// pbmi	A pointer to a BITMAPINFO structure that specifies various attributes of the DIB
                    DIB_RGB_COLORS,	// usage The type of data contained in the bmiColors array member of the BITMAPINFO
                    (void **)&pframebuf,// ppvBits A pointer to a variable that receives a pointer to the location of the DIB bit values.
                    NULL, 		// hSection A handle to a file-mapping object that the function will use to create the DIB. This parameter can be NULL.
                    0			// offset
                );
                if (debug) printf("          CreateDIBSection=%p, pframebuf=%p\n", gBitmap, pframebuf);
                if (pframebuf) memcpy(pframebuf, &framebuf, sizeof(*pframebuf));
                HGDIOBJ old = SelectObject(hdcMem, gBitmap);
                DeleteObject(old); // release the 1x1@1bpp default bitmap
                //crBkgnd = GetBkColor(hdc);
                //hbrBkgnd = CreateSolidBrush(crBkgnd);
                ReleaseDC(hWnd, hdc);

            } else if (method == 3) {
                pframebuf = &framebuf;
                if (debug) printf("          gBitmap=%p, pframebuf=%p\n", gBitmap, pframebuf);

            }

            if (!benchmark) {
                DWORD tid;
                gThread = CreateThread(
                    NULL,                   // default security attributes
                    0,                      // use default stack size
                    asm_main_call,          // thread function name
                    NULL,                   // argument to thread function
                    0,                      // use default creation flags
                    &tid                    // returns the thread identifier
                );
                if (debug) printf("=== Created thread %p id=%ld\n", gThread, tid);
            }

            if (!benchmark) {
                gTimer1 = SetTimer(
                    hWnd,        	// handle to main window
                    IDT_TIMER1,       	// timer identifier
                    FPS_DELAY,		// unsigned interval in ms
                    (TIMERPROC) NULL  	// no timer callback
                );
                if (debug) printf("=== Created timer %x\n", gTimer1);
            }

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
            if (debug) printf("WM_DESTROY\n");

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

            if (method == 1) {
                DeleteObject(gBitmap);
                DeleteDC(hdcMem);
            } else if (method == 2) {
                DeleteObject(gBitmap);
                DeleteDC(hdcMem);
            } else if (method == 3) {
            }

            PostQuitMessage(0);
            return 0;
        }

        case WM_USER: {
            if (!InSendMessage())
                return 0; //ReplyMessage(1);

            usleep(keydelay);

            if (wParam) {
                MSG msg = {};
                // WM_KEYFIRST 0x0100 WM_KEYDOWN 0x0100 WM_KEYUP 0x0101 WM_CHAR 0x0102 WM_DEADCHAR 0x0103 WM_SYSKEYDOWN 0x0104
                // WM_SYSKEYUP 0x0105 WM_SYSCHAR 0x0106 WM_SYSDEADCHAR 0x0107 WM_UNICHAR 0x0109 WM_KEYLAST 0x0109
                // WM_MOUSEFIRST 0x0200 WM_MOUSEMOVE 0x0200 WM_LBUTTONDOWN 0x0201 WM_LBUTTONUP 0x0202 WM_LBUTTONDBLCLK 0x0203
                // WM_RBUTTONDOWN 0x0204 WM_RBUTTONUP 0x0205 WM_RBUTTONDBLCLK 0x0206 WM_MBUTTONDOWN 0x0207 WM_MBUTTONUP 0x0208
                // WM_MBUTTONDBLCLK 0x0209 WM_MOUSEWHEEL 0x020A WM_XBUTTONDOWN 0x020B WM_XBUTTONUP 0x020C WM_XBUTTONDBLCLK 0x020D WM_MOUSELAST 0x020D
                if (GetMessage(&msg, hWnd, WM_KEYDOWN/*Min*/, WM_KEYDOWN/*Max*/)) {
                    return msg.wParam;
                }
                return 0;
            } else {
                MSG msg = {};
                if (PeekMessage(&msg, hWnd, WM_KEYDOWN/*Min*/, WM_KEYDOWN/*Max*/, PM_NOREMOVE)) {
                    return msg.wParam;
                }
                return 0;
            }
            break;
        }

        case WM_TIMER: {
            switch (wParam) {
                case IDT_TIMER1:
                    if (debug > 1) printf("[%" PRIu64 "] WM_TIMER\n", GetTickCount64());
                    // process the framebuffer refresh timer (vsync-like)
                    InvalidateRect(hWnd, NULL, false);
                    return 0;
                case IDT_TIMER2:
                    // process the five-minute timer
                    return 0;
            }
            break;
        }

        case WM_NCPAINT: {
            static int cnt;
            if (debug) printf("WM_NCPAINT %d\n", cnt++);
            // The DefWindowProc function paints the window frame.
            break;
        }

        case WM_PAINT: {
            DwmFlush();
            // IDXGIOutput::WaitForVBlank(); IDXGIFactory2::CreateSwapChainForHwnd() IDXGISwapChain2::GetFrameLatencyWaitableObject() D3DKMTOpenAdapterFromHdc D3DKMTWaitForVerticalBlankEvent
            /*
            static double prevtc = 0, prevdt = 0;
            ULONGLONG tcl; QueryUnbiasedInterruptTime(&tcl);
            double tc = tcl / 10000.0; // convert 100ns to 1ms
            double dt = tc - prevtc;
            if (debug && abs(dt - prevdt) > 3)
                printf("[%" PRIu64 "] WM_PAINT [%.2f] frame = %.2f ms\n", GetTickCount64(), tc, dt);
            prevdt = dt;
            prevtc = tc;
            */
            /*
            DWM_TIMING_INFO ti;
            typedef struct _DWM_TIMING_INFO {
              UINT32          cbSize;			The size of this DWM_TIMING_INFO structure.
              UNSIGNED_RATIO  rateRefresh;              The monitor refresh rate.
              QPC_TIME        qpcRefreshPeriod;         The monitor refresh period.
              UNSIGNED_RATIO  rateCompose;              The composition rate.
              QPC_TIME        qpcVBlank;                The query performance counter value before the vertical blank.
              DWM_FRAME_COUNT cRefresh;                 The DWM refresh counter.
              UINT            cDXRefresh;               The DirectX refresh counter.
              QPC_TIME        qpcCompose;               The query performance counter value for a frame composition.
              DWM_FRAME_COUNT cFrame;                   The frame number that was composed at qpcCompose.
              UINT            cDXPresent;               The DirectX present number used to identify rendering frames.
              DWM_FRAME_COUNT cRefreshFrame;            The refresh count of the frame that was composed at qpcCompose.
              DWM_FRAME_COUNT cFrameSubmitted;          The DWM frame number that was last submitted.
              UINT            cDXPresentSubmitted;      The DirectX present number that was last submitted.
              DWM_FRAME_COUNT cFrameConfirmed;          The DWM frame number that was last confirmed as presented.
              UINT            cDXPresentConfirmed;      The DirectX present number that was last confirmed as presented.
              DWM_FRAME_COUNT cRefreshConfirmed;        The target refresh count of the last frame confirmed as completed by the GPU.
              UINT            cDXRefreshConfirmed;      The DirectX refresh count when the frame was confirmed as presented.
              DWM_FRAME_COUNT cFramesLate;              The number of frames the DWM presented late.
              UINT            cFramesOutstanding;       The number of composition frames that have been issued but have not been confirmed as completed.
              DWM_FRAME_COUNT cFrameDisplayed;          The last frame displayed.
              QPC_TIME        qpcFrameDisplayed;        The QPC time of the composition pass when the frame was displayed.
              DWM_FRAME_COUNT cRefreshFrameDisplayed;   The vertical refresh count when the frame should have become visible.
              DWM_FRAME_COUNT cFrameComplete;           The ID of the last frame marked as completed.
              QPC_TIME        qpcFrameComplete;         The QPC time when the last frame was marked as completed.
              DWM_FRAME_COUNT cFramePending;            The ID of the last frame marked as pending.
              QPC_TIME        qpcFramePending;          The QPC time when the last frame was marked as pending.
              DWM_FRAME_COUNT cFramesDisplayed;         The number of unique frames displayed. This value is valid only after a second call to the DwmGetCompositionTimingInfo function.
              DWM_FRAME_COUNT cFramesComplete;          The number of new completed frames that have been received.
              DWM_FRAME_COUNT cFramesPending;           The number of new frames submitted to DirectX but not yet completed.
              DWM_FRAME_COUNT cFramesAvailable;         The number of frames available but not displayed, used, or dropped. This value is valid only after a second call to DwmGetCompositionTimingInfo.
              DWM_FRAME_COUNT cFramesDropped;           The number of rendered frames that were never displayed because composition occurred too late. This value is valid only after a second call to DwmGetCompositionTimingInfo.
              DWM_FRAME_COUNT cFramesMissed;            The number of times an old frame was composed when a new frame should have been used but was not available.
              DWM_FRAME_COUNT cRefreshNextDisplayed;    The frame count at which the next frame is scheduled to be displayed.
              DWM_FRAME_COUNT cRefreshNextPresented;    The frame count at which the next DirectX present is scheduled to be displayed.
              DWM_FRAME_COUNT cRefreshesDisplayed;      The total number of refreshes that have been displayed for the application since the DwmSetPresentParameters function was last called.
              DWM_FRAME_COUNT cRefreshesPresented;      The total number of refreshes that have been presented by the application since DwmSetPresentParameters was last called.
              DWM_FRAME_COUNT cRefreshStarted;          The refresh number when content for this window started to be displayed.
              ULONGLONG       cPixelsReceived;          The total number of pixels DirectX redirected to the DWM.
              ULONGLONG       cPixelsDrawn;             The number of pixels drawn.
              DWM_FRAME_COUNT cBuffersEmpty;            The number of empty buffers in the flip chain.
            } DWM_TIMING_INFO;
            ti.cbSize = sizeof(ti);
            DWMAPI DwmGetCompositionTimingInfo(hWnd, DWM_TIMING_INFO *pTimingInfo);
            */

            //SetWindowHandle(hWnd);
            PAINTSTRUCT ps = {};
            HDC hdc = BeginPaint(hWnd, &ps);
            /*
            if (memcmp(&oldframebuf, pframebuf, sizeof(oldframebuf)) != 0) {
                BitBlt(hdc, 0, 0, FB_WIDTH, FB_HEIGHT, hdcMem, 0, 0, SRCCOPY);
                memcpy(&oldframebuf, pframebuf, sizeof(oldframebuf));
            }
            */

            RECT rc = {};
            GetClientRect(hWnd, &rc);
            static int cnt;
            if (debug > 1) printf("[%" PRIu64 "] WM_PAINT %d ps.rcPaint=[%ld,%ld - %ld,%ld] GetClientRect=%ldx%ld InSendMessage=%d\n",
                GetTickCount64(), cnt++, ps.rcPaint.left, ps.rcPaint.top, ps.rcPaint.right, ps.rcPaint.bottom,
                rc.right - rc.left, rc.bottom - rc.top, InSendMessage()
            );
                    if (debug > 1) printf("[%" PRIu64 "] WM_TIMER\n", GetTickCount64());
            //ps.rcPaint = rc;

            BITMAPINFO bmi = {};
            bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
            bmi.bmiHeader.biWidth = FB_WIDTH;
            bmi.bmiHeader.biHeight = -FB_HEIGHT; // top-down
            bmi.bmiHeader.biPlanes = 1;
            bmi.bmiHeader.biBitCount = 8*4;
            bmi.bmiHeader.biCompression = BI_RGB;
            bmi.bmiHeader.biSizeImage = ((((FB_WIDTH * bmi.bmiHeader.biBitCount) + 31) & ~31) >> 3) * FB_HEIGHT;

            //SetViewportOrgEx(hdc, 0, 0, NULL);
            //SetViewportExtEx(hdc, rc.right, rc.bottom, NULL);

            //RECT fr = { start, start, start+200, start+200 };
            //FillRect(hdc, &fr, (HBRUSH) (COLOR_WINDOW+1));
            //FillRect(hdc, &ps.rcPaint, (HBRUSH) (COLOR_WINDOW+1));

            //HBRUSH hbrOld = (HBRUSH) SelectObject(hDC, GetStockObject(WHITE_BRUSH));
            //Ellipse(hDC, rc.left, rc.top, rc.right, rc.bottom);
            //SelectObject(hDC, hbrOld);

            //HGDIOBJ old = SelectObject(hdcMem, hBitmap);
            //SelectObject(hdcMem, old);
            //DeleteObject(hBitmap);
            //GetObject(hBitmap, sizeof(bitmap), &bitmap);

            if (method == 1) {

                int res = SetDIBits(hdcMem, gBitmap, 0, FB_HEIGHT, (void *)pframebuf, &bmi, DIB_RGB_COLORS);
                if (debug > 1) printf("         SetDIBits=%d\n", res);

                // Copy image from temp HDC to window
                BitBlt(hdc, // Destination
                    0,      // x and
                    0,      // y - upper-left corner of place, where we'd like to copy
                    FB_WIDTH,    // width of the region
                    FB_HEIGHT,   // height
                    hdcMem, // source
                    0,      // x and
                    0,      // y of upper left corner  of part of the source, from where we'd like to copy
                    SRCCOPY // Defined DWORD to juct copy pixels. Watch more on msdn;
                );

            } else if (method == 2) {

                // Copy image from temp HDC to window
                BitBlt(hdc, // Destination
                    0,      // x and
                    0,      // y - upper-left corner of place, where we'd like to copy
                    FB_WIDTH,    // width of the region
                    FB_HEIGHT,   // height
                    hdcMem, // source
                    0,      // x and
                    0,      // y of upper left corner  of part of the source, from where we'd like to copy
                    SRCCOPY // Defined DWORD to juct copy pixels. Watch more on msdn;
                );

            } else if (method == 3) {

                // Copy array
                int res = SetDIBitsToDevice(
                    hdc,		// hdc	A handle to the device context.
                    0,			// xDest	The x-coordinate, in logical units, of the upper-left corner of the destination rectangle.
                    0,			// yDest	The y-coordinate, in logical units, of the upper-left corner of the destination rectangle.
                    FB_WIDTH,		// w		The width, in logical units, of the image.
                    FB_HEIGHT,		// h		The height, in logical units, of the image.
                    0,			// xSrc		The x-coordinate, in logical units, of the lower-left corner of the image.
                    0,			// ySrc		The y-coordinate, in logical units, of the lower-left corner of the image.
                    0,			// StartSc	The starting scan line in the image.
                    FB_HEIGHT,		// cLines	The number of DIB scan lines contained in the array pointed to by the lpvBits parameter.
                    (void *) pframebuf,	// lpvBits	A pointer to the color data stored as an array of bytes. For more information, see the following Remarks section.
                    &bmi,		// lpbmi	A pointer to a BITMAPINFO structure that contains information about the DIB.
                    DIB_RGB_COLORS	// ColorUse
                );
                if (debug > 1) printf("         SetDIBitsToDevice=%d\n", res);

            }

            EndPaint(hWnd, &ps);
            MSG msgtmp; // drain WM_PAINT messages
            if (debug > 1) printf("[%" PRIu64 "] WM_PAINT drain\n", GetTickCount64());
            if (cnt > 3) while (PeekMessage(&msgtmp, NULL, WM_PAINT, WM_PAINT, PM_REMOVE | PM_NOYIELD | PM_QS_PAINT));
            if (debug > 1) printf("[%" PRIu64 "] WM_PAINT drain end\n", GetTickCount64());
            if (getkey_q.size() <= 1) { getkey_q.put(Key::NextFrame); nredraws++; }
            if (debug > 1) printf("[%" PRIu64 "] WM_PAINT end\n", GetTickCount64());
            return 0;
        }

        case WM_ERASEBKGND: {
            static int cnt;
            if (debug) printf("WM_ERASEBKGND %d is_moving=%d\n", cnt++, is_moving);
            if (0) {
                HDC hdc = (HDC) wParam;
                RECT rc;
                GetClientRect(hWnd, &rc);
                SetMapMode(hdc, MM_ANISOTROPIC);
                SetWindowExtEx(hdc, 100, 100, NULL);
                SetViewportExtEx(hdc, rc.right, rc.bottom, NULL);
                HBRUSH hbrWhite = (HBRUSH) GetStockObject(WHITE_BRUSH);
                HBRUSH hbrGray = (HBRUSH) GetStockObject(GRAY_BRUSH);
                FillRect(hdc, &rc, hbrWhite); // hbrWhite
                for (int i = 0; i < 13; i++) {
                    int x = (i * 40) % 100;
                    int y = ((i * 40) / 100) * 20;
                    SetRect(&rc, x, y, x + 20, y + 20);
                    FillRect(hdc, &rc, hbrGray);
                }
                // An application should return nonzero if it erases the background
                return 1;
            }
            break;
        }

        case WM_SETREDRAW: {
            static int cnt;
            if (debug) printf("WM_SETREDRAW(%d) %d\n", wParam, cnt++);
            if (InSendMessage()) ReplyMessage(true);
            if (wParam) {
                //RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);
                //UpdateWindow(hWnd);
            }
            break;
        }

        case WM_NCHITTEST: {
            static int cnt;
            if (debug) printf("WM_NCHITTEST %d at %u,%u is_moving=%d\n", cnt++,
                GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), is_moving);
            /*
            The return value of the DefWindowProc function is one of the following values, indicating the position of the cursor hot spot.
            HTBORDER 18 - In the border of a window that does not have a sizing border.
            HTBOTTOM 15 - In the lower-horizontal border of a resizable window (the user can click the mouse to resize the window vertically).
            HTBOTTOMLEFT 16 - In the lower-left corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
            HTBOTTOMRIGHT 17 - In the lower-right corner of a border of a resizable window (the user can click the mouse to resize the window diagonally).
            HTCAPTION 2 - In a title bar.
            HTCLIENT 1 - In a client area.
            HTCLOSE 20 - In a Close button.
            HTERROR -2 - On the screen background or on a dividing line between windows (same as HTNOWHERE, except that the DefWindowProc function produces a system beep to indicate an error).
            HTGROWBOX 4 - In a size box (same as HTSIZE).
            HTHELP 21 - In a Help button.
            HTHSCROLL 6 - In a horizontal scroll bar.
            HTLEFT 10 - In the left border of a resizable window (the user can click the mouse to resize the window horizontally).
            HTMENU 5 - In a menu.
            HTMAXBUTTON 9 - In a Maximize button.
            HTMINBUTTON 8 - In a Minimize button.
            HTNOWHERE 0 - On the screen background or on a dividing line between windows.
            HTREDUCE 8 - In a Minimize button.
            HTRIGHT 11 - In the right border of a resizable window (the user can click the mouse to resize the window horizontally).
            HTSIZE 4 - In a size box (same as HTGROWBOX).
            HTSYSMENU 3 - In a window menu or in a Close button in a child window.
            HTTOP 12 - In the upper-horizontal border of a window.
            HTTOPLEFT 13 - In the upper-left corner of a window border.
            HTTOPRIGHT 14 - In the upper-right corner of a window border.
            HTTRANSPARENT -1 - In a window currently covered by another window in the same thread (the message will be sent to underlying windows in the same thread until one of them returns a code that is not HTTRANSPARENT).
            HTVSCROLL 7 - In the vertical scroll bar.
            HTZOOM 9 - In a Maximize button.
            */
            //return HTCAPTION;
            break;
        }

        case WM_ENTERSIZEMOVE: {
            static int cnt;
            if (debug) printf("WM_ENTERSIZEMOVE %d swap=%d\n", cnt++, SwapBuffers(GetDC(hWnd)));
            is_moving = 1;
            SendMessage(hWnd, WM_SETREDRAW, false, 0);
            break;
        }

        case WM_EXITSIZEMOVE: {
            static int cnt;
            if (debug) printf("WM_EXITSIZEMOVE %d swap=%d\n", cnt++, SwapBuffers(GetDC(hWnd)));

            RECT rect;
            GetClientRect(hWnd, &rect);
            InvalidateRect(hWnd, &rect, false);

            SendMessage(hWnd, WM_SETREDRAW, true, 0);
            is_moving = 0;
            break;
        }

        case WM_MOVE:
        case WM_SIZE: {
            static int cnt;
            if (debug) printf("WM_MOVE|WM_SIZE %d is_moving=%d\n", cnt++, is_moving);
            if (debug) printf("WM_MOVE|WM_SIZE wParam=%d rcClient was [%ld,%ld - %ld,%ld]\n", wParam, rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

            RECT rect;
            GetClientRect(hWnd, &rect);
            POINT ptClientUL = { rect.left, rect.top };
            ClientToScreen(hWnd, &ptClientUL);
            POINT ptClientLR = { rect.right, rect.bottom };
            ClientToScreen(hWnd, &ptClientLR);
            rcClient = { ptClientUL.x, ptClientUL.y, ptClientLR.x, ptClientLR.y };
            if (debug) printf("                rcClient=[%ld,%ld - %ld,%ld]\n", rcClient.left, rcClient.top, rcClient.right, rcClient.bottom);

            if (is_moving) SendMessage(hWnd, WM_SETREDRAW, true, 0);
            RedrawWindow(hWnd, &rect, NULL, RDW_NOERASE | RDW_NOFRAME | RDW_INVALIDATE | RDW_UPDATENOW | RDW_NOCHILDREN);
            if (is_moving) SendMessage(hWnd, WM_SETREDRAW, false, 0);
            break;
        }

        case WM_MOUSEMOVE: {
            int state = GET_KEYSTATE_WPARAM(wParam);
            int button = GET_XBUTTON_WPARAM(wParam);
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (debug > 1) printf("%s state=%#04x button=%#04x xy=%d,%d\n",
                debWin_msg(uMsg), state, button, x, y);
            int key = Key::MouseMove | (x & 2047) << 9 | (y & 2047) << 20;
            getkey_q.put(key);
            return 0;
        }
        case WM_MOUSEWHEEL: {
            int state = GET_KEYSTATE_WPARAM(wParam);
            int delta = int(wParam) >> 16;
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (debug) printf("%s state=%#04x delta=%d xy=%d,%d\n",
                debWin_msg(uMsg), state, delta, x, y);

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
            int button = GET_XBUTTON_WPARAM(wParam);
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (debug) printf("%s state=%#04x button=%#04x xy=%d,%d\n",
                debWin_msg(uMsg), state, button, x, y);

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
            int button = GET_XBUTTON_WPARAM(wParam);
            int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);
            if (debug) printf("%s state=%#04x button=%#04x xy=%d,%d\n",
                debWin_msg(uMsg), state, button, x, y);

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
            const int isDown = (lParam >> 30) & 1;

            char vkStr[64] = {};
            int vkLen = GetKeyNameText(lParam, vkStr, sizeof(vkStr));

            if (debug) printf("%s vk[%d]=%s %d scan=%d extKey=%d isDown=%d\n",
                debWin_msg(uMsg), vkLen, vkStr, vk, scan, extKey, isDown);

            BYTE keyboardState[256] = {};
            GetKeyboardState(keyboardState);
            if (debug) { printf("keyboardState:"); for (int i = 0; i < 256; i++) if (keyboardState[i]&128) printf(" %d", i); printf("\n"); }

            wchar_t unicode[32] = {};
            int ulen = ToUnicode(vk, scan, keyboardState, unicode, ARRAY_SIZE(unicode), 0);
            if (debug) printf("ToUnicode() len=%d unicode=%x %x %x\n", ulen, unicode[0], unicode[1], unicode[2]);

            WORD ascii = 0;
            int alen = ToAscii(vk, scan, keyboardState, &ascii, 0);
            if (debug) printf("ToAscii() len=%d ascii='%c' %x\n", alen, ascii, ascii);

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

        case WM_CHAR: {
            int c = wParam;
            if (debug) printf("WM_CHAR '%c'\n", c);
            switch (c) {
                case 'q':
                    // Quit
                    DestroyWindow(hWnd);
                    return 0;
                case 'r':
                    // Redraw
                    InvalidateRect(hWnd, NULL, false);
                    return 0;
                case 'p':
                    if (!pframebuf) return 0;
                    // Paint
                    {
                        unsigned char color = (start & 0xff) << 12;
                        memset(pframebuf, sizeof(*pframebuf), color);

                        int maxx = std::min(start + 200, FB_WIDTH);
                        int maxy = std::min(start + 200, FB_HEIGHT);
                        for (int y = start; y < maxy; y++) {
                            for (int x = start; x < maxx; x++) {
                                pframebuf[0][y][x] =
                                    (x==start || x==maxx-1 || y==start || y==maxy-1) ?
                                    0x00ff1133 : 0x0011ff11; // 00 rr gg bb
                            }
                        }

                        start += 4;
                    }
                    InvalidateRect(hWnd, NULL, false);
                    return 0;
            }
            break;
        }

        case WM_COMMAND: {
            if (debug) printf("WM_COMMAND\n");
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
            if (debug) printf("WM_APP\n");
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

        case WM_NCMOUSEMOVE:
        case WM_SETCURSOR:
            break;

        default:
            if (debug) printf("WndProc default uMsg=%s (%u), wParam=%d, lParam=%lx\n",
                debWin_msg(uMsg), uMsg, wParam, lParam);
            break;
    }

    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

// Entry point

int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE /*hPrevInstance*/, LPTSTR lpCmdLine, int nCmdShow)
{
    HWND hPrev = NULL;
    int nloops = -1;

    if (debug) printf("lpCmdLine=%s\n", lpCmdLine);
    if (lpCmdLine[0] >= '0' && lpCmdLine[0] <= '9') method = lpCmdLine[0] - '0';
    if (strstr(lpCmdLine, "-d")) debug = 1;
    if (strstr(lpCmdLine, "-dd")) debug = 2;
    if (strstr(lpCmdLine, "-b")) { benchmark = 1; debug = 0; }
    if (strstr(lpCmdLine, "-c")) nloops = 100;
    if (strstr(lpCmdLine, "-k")) nloops = 1000;

    if ((hPrev = FindWindow(TEXT(THIS_CLASSNAME), TEXT(THIS_TITLE))) != 0)  {
        MessageBox(NULL, TEXT("Previous instance alredy running!"), TEXT("Warning"), MB_OK);
        return 0;
    }

    {
        void *h_mapping = CreateFileMapping(
            INVALID_HANDLE_VALUE,		// not bound to actual file
            NULL,				// default security
            PAGE_EXECUTE_READWRITE | SEC_COMMIT,// access flags
            0,					// dwMaximumSizeHigh
            4096,				// dwMaximumSizeLow
            NULL				// no name
        );
        if (h_mapping == NULL) {
            char *msg;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
            printf("CreateFileMapping error: %s\n", msg);
            LocalFree(msg);
            exit(-1);
        }
        void *p_view = MapViewOfFileEx(
            h_mapping,                  // mapping handle
            FILE_MAP_READ | FILE_MAP_WRITE | FILE_MAP_EXECUTE, // map flags
            0,                          // offset within the file mapping high
            0,				// offset within the file mapping low
            0,				// number of bytes of a file mapping to map to a view; if 0, the mapping extends from the specified offset to the end of the file mapping
            (void*)0x20000000		// 0x00370000: pointer to the memory address in the calling process address space where mapping begins
        );
        if (p_view == NULL) {
            char *msg;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
            printf("MapViewOfFile error: %s\n", msg);
            LocalFree(msg);
            exit(-2);
        }

        printf("=== mmap=%p\n", p_view);

        if (UnmapViewOfFile(p_view) == false) {
            char *msg;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
            printf("UnmapViewOfFile error: %s\n", msg);
            LocalFree(msg);
            exit(-3);
        }
        if (CloseHandle(h_mapping) == false) {
            char *msg;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, GetLastError(), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR) &msg, 0, NULL);
            printf("CloseHandle error: %s\n", msg);
            LocalFree(msg);
            exit(-4);
        }
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
    //wclx.hIcon        = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_TRAYICON));
    //wclx.hIconSm      = LoadSmallIcon(hInstance, IDI_TRAYICON);
    wclx.hCursor        = LoadCursor(NULL, IDC_ARROW);
    wclx.hbrBackground  = NULL;
    //wclx.hbrBackground  = (HBRUSH) (COLOR_BTNFACE + 1);
    //wclx.hbrBackground  = (HBRUSH) GetStockObject(BLACK_BRUSH);
    wclx.hbrBackground  = CreateSolidBrush(0x333333);
    wclx.lpszMenuName   = NULL;
    wclx.lpszClassName  = TEXT(THIS_CLASSNAME);
    RegisterClassEx(&wclx);

    /*
    DWORD dwStyle = GetWindowLongPtr(hWnd, GWL_STYLE);
    DWORD dwExStyle = GetWindowLongPtr(hWnd, GWL_EXSTYLE);
    HMENU menu = GetMenu(hWnd);
    */
    DWORD dwStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX;
    DWORD dwExStyle = 0; //WS_EX_OVERLAPPEDWINDOW | WS_EX_APPWINDOW | WS_EX_NOACTIVATE;

    RECT clientarea = { 0, 0, FB_WIDTH, FB_HEIGHT };
    if (!AdjustWindowRectEx(&clientarea, dwStyle/*GetWindowLong(hWnd, GWL_STYLE)*/, 0/*GetMenu(hWnd) != 0*/, dwExStyle/*GetWindowLong(hWnd, GWL_EXSTYLE)*/)) {
        if (debug) printf("AdjustWindowRectEx() FAILED rect = { %ld, %ld, %ld, %ld  }\n",
            clientarea.left, clientarea.top, clientarea.right, clientarea.bottom);
        clientarea = { 0, 0, FB_WIDTH + 8, FB_HEIGHT + 27 };
    } else {
        if (debug) printf("AdjustWindowRectEx() rect = { %ld, %ld, %ld, %ld  }\n",
            clientarea.left, clientarea.top, clientarea.right, clientarea.bottom);
    }

    HWND hWnd = gWnd = CreateWindowEx(
        dwExStyle,		// Extended possibilites for variation
        TEXT(THIS_CLASSNAME),	// Classname
        TEXT(THIS_TITLE),	// Title Text
        dwStyle,		// default window
        CW_USEDEFAULT,		// Windows decides the position
        CW_USEDEFAULT,		// Where the window ends up on the screen
        clientarea.right - clientarea.left, // width and
        clientarea.bottom - clientarea.top, // height in pixels
        HWND_DESKTOP,		// The window is a child-window to desktop
        NULL,			// No menu
        hInstance,		// Program Instance handler
        NULL			// No Window Creation data
    );

    if (!hWnd) {
        MessageBox(NULL, "Can't create window!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
        return 1;
    }

    // If you have changed certain window data using SetWindowLong, you must
    // call SetWindowPos for the changes to take effect. Use the following
    // combination for uFlags: SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_FRAMECHANGED
    // https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-setwindowpos
    SetWindowLong(hWnd, GWL_STYLE, GetWindowLong(hWnd, GWL_STYLE) & ~WS_SIZEBOX & ~WS_MAXIMIZEBOX);

    // Make window semi-transparent, and mask out background color
    //SetLayeredWindowAttributes(hWnd, RGB(0, 0, 0), 128, LWA_ALPHA | LWA_COLORKEY);
    ShowWindow(hWnd, nCmdShow);
    GdiFlush();

    // UpdateWindow() will force a redraw of only the update region of the window,
    // i.e. that part of the window that has been invalidated (e.g. by calling InvalidateRect)
    // since the last paint cycle.
    // The UpdateWindow function updates the client area of the specified window by sending a WM_PAINT message to the window if the window's update region is not empty. The function sends a WM_PAINT message directly to the window procedure of the specified window, bypassing the application queue. If the update region is empty, no message is sent.
    UpdateWindow(hWnd);

    if (!SetPriorityClass(GetCurrentProcess(), ABOVE_NORMAL_PRIORITY_CLASS)) {
        MessageBox(NULL, "Can't set priority class!", TEXT("Warning!"), MB_ICONERROR | MB_OK | MB_TOPMOST);
        return 1;
    }
    // BOOL InvalidateRect(HWND hWnd, const RECT *lpRect, BOOL bErase);
    // Adds a rectangle to the specified window's update region
    // 	hWnd - A handle to the window whose update region has changed. If this parameter is NULL, the system invalidates and redraws all windows, not just the windows for this application, and sends the WM_ERASEBKGND and WM_NCPAINT messages before the function returns. Setting this parameter to NULL is not recommended.
    // 	lpRect - if NULL, the entire client area is added to the update region
    //	bErase - if TRUE, the background is erased when the BeginPaint function is called
    //InvalidateRect(hWnd, NULL, false);

    // BOOL GetUpdateRect(HWND hWnd, LPRECT lpRect, BOOL bErase);
    // Retrieves the coordinates of the smallest rectangle that completely encloses the update region of the specified window. GetUpdateRect retrieves the rectangle in logical coordinates. If there is no update region, GetUpdateRect retrieves an empty rectangle (sets all coordinates to zero).
    // The update rectangle retrieved by the BeginPaint function is identical to that retrieved by GetUpdateRect.
    // BeginPaint automatically validates the update region, so any call to GetUpdateRect made immediately after the call to BeginPaint retrieves an empty update region.
    // hWnd - Handle to the window whose update region is to be retrieved.
    // lpRect - Pointer to the RECT structure that receives the coordinates, in device units, of the enclosing rectangle.
    // An application can set this parameter to NULL to determine whether an update region exists for the window. If this parameter is NULL, GetUpdateRect returns nonzero if an update region exists, and zero if one does not. This provides a simple and efficient means of determining whether a WM_PAINT message resulted from an invalid area.
    // bErase - Specifies whether the background in the update region is to be erased. If this parameter is TRUE and the update region is not empty, GetUpdateRect sends a WM_ERASEBKGND message to the specified window to erase the background.
    // Return value. If the update region is not empty, the return value is nonzero.

    // BOOL RedrawWindow(HWND hWnd, const RECT *lprcUpdate, HRGN hrgnUpdate, UINT flags);
    // The RedrawWindow function updates the specified rectangle or region in a window's client area.
    // hWnd - A handle to the window to be redrawn. If this parameter is NULL, the desktop window is updated.
    // lprcUpdate - A pointer to a RECT structure containing the coordinates, in device units, of the update rectangle. This parameter is ignored if the hrgnUpdate parameter identifies a region.
    // hrgnUpdate - A handle to the update region. If both the hrgnUpdate and lprcUpdate parameters are NULL, the entire client area is added to the update region.
    // flags - One or more redraw flags. This parameter can be used to invalidate or validate a window, control repainting, and control which windows are affected by RedrawWindow.
    // The following flags are used to invalidate the window.
    // RDW_ERASE Causes the window to receive a WM_ERASEBKGND message when the window is repainted. The RDW_INVALIDATE flag must also be specified; otherwise, RDW_ERASE has no effect.
    // RDW_FRAME Causes any part of the nonclient area of the window that intersects the update region to receive a WM_NCPAINT message. The RDW_INVALIDATE flag must also be specified; otherwise, RDW_FRAME has no effect. The WM_NCPAINT message is typically not sent during the execution of RedrawWindow unless either RDW_UPDATENOW or RDW_ERASENOW is specified.
    // RDW_INTERNALPAINT Causes a WM_PAINT message to be posted to the window regardless of whether any portion of the window is invalid.
    // RDW_INVALIDATE Invalidates lprcUpdate or hrgnUpdate (only one may be non-NULL). If both are NULL, the entire window is invalidated.
    // The following flags are used to validate the window.
    // RDW_NOERASE Suppresses any pending WM_ERASEBKGND messages.
    // RDW_NOFRAME Suppresses any pending WM_NCPAINT messages. This flag must be used with RDW_VALIDATE and is typically used with RDW_NOCHILDREN. RDW_NOFRAME should be used with care, as it could cause parts of a window to be painted improperly.
    // RDW_NOINTERNALPAINT Suppresses any pending internal WM_PAINT messages. This flag does not affect WM_PAINT messages resulting from a non-NULL update area.
    // RDW_VALIDATE Validates lprcUpdate or hrgnUpdate (only one may be non-NULL). If both are NULL, the entire window is validated. This flag does not affect internal WM_PAINT messages.
    // The following flags control when repainting occurs. RedrawWindow will not repaint unless one of these flags is specified.
    // RDW_ERASENOW Causes the affected windows (as specified by the RDW_ALLCHILDREN and RDW_NOCHILDREN flags) to receive WM_NCPAINT and WM_ERASEBKGND messages, if necessary, before the function returns. WM_PAINT messages are received at the ordinary time.
    // RDW_UPDATENOW Causes the affected windows (as specified by the RDW_ALLCHILDREN and RDW_NOCHILDREN flags) to receive WM_NCPAINT, WM_ERASEBKGND, and WM_PAINT messages, if necessary, before the function returns.
    // By default, the windows affected by RedrawWindow depend on whether the specified window has the WS_CLIPCHILDREN style. Child windows that are not the WS_CLIPCHILDREN style are unaffected; non-WS_CLIPCHILDREN windows are recursively validated or invalidated until a WS_CLIPCHILDREN window is encountered. The following flags control which windows are affected by the RedrawWindow function.
    // RDW_ALLCHILDREN Includes child windows, if any, in the repainting operation.
    // RDW_NOCHILDREN Excludes child windows, if any, from the repainting operation.
    // Return value. If the function succeeds, the return value is nonzero.

    //RedrawWindow(hWnd, NULL, NULL, RDW_ERASE | RDW_FRAME | RDW_INVALIDATE | RDW_ALLCHILDREN);
    //RedrawWindow(hWnd, NULL, NULL, RDW_INVALIDATE);

    // BOOL WaitMessage();
    // Yields control to other threads when a thread has no other messages in its message queue. The WaitMessage function suspends the thread and does not return until a new message is placed in the thread's message queue.
    // If the function succeeds, the return value is nonzero.

    // BOOL GdiFlush();
    // Flushes the calling thread's current batch.
    // Return value. If not all functions in the current batch succeed, the return value is zero, indicating that at least one function returned an error.
    // Batching enhances drawing performance by minimizing the amount of time needed to call GDI drawing functions that return Boolean values. The system accumulates the parameters for calls to these functions in the current batch and then calls the functions when the batch is flushed by any of the following means:
    // * Calling the GdiFlush function. * Reaching or exceeding the batch limit set by the GdiSetBatchLimit function. * Filling the batching buffers. * Calling any GDI function that does not return a Boolean value.
    // The return value for GdiFlush applies only to the functions in the batch at the time GdiFlush is called. Errors that occur when the batch is flushed by any other means are never reported.
    // Note. The batch limit is maintained for each thread separately. In order to completely disable batching, call GdiSetBatchLimit (1) during the initialization of each thread.
    // An application should call GdiFlush before a thread goes away if there is a possibility that there are pending function calls in the graphics batch queue. The system does not execute such batched functions when a thread goes away.
    // A multithreaded application that serializes access to GDI objects with a mutex must ensure flushing the GDI batch queue by calling GdiFlush as each thread releases ownership of the GDI object. This prevents collisions of the GDI objects (device contexts, metafiles, and so on).

    // DWORD GdiGetBatchLimit();
    // Returns the maximum number of function calls that can be accumulated in the calling thread's current batch. The system flushes the current batch whenever this limit is exceeded.

    // DWORD GdiSetBatchLimit(DWORD dw);
    // Sets the maximum number of function calls that can be accumulated in the calling thread's current batch. The system flushes the current batch whenever this limit is exceeded.
    // dw - Specifies the batch limit to be set. A value of 0 sets the default limit. A value of 1 disables batching.

    // BOOL LockWindowUpdate(HWND hWndLock);
    // Disables or enables drawing in the specified window. Only one window can be locked at a time.
    // hWndLock - The window in which drawing will be disabled. If this parameter is NULL, drawing in the locked window is enabled.
    // If the function fails, the return value is zero, indicating that an error occurred or another window was already locked.
    // The purpose of the LockWindowUpdate function is to permit drag/drop feedback to be drawn over a window without interference from the window itself. The intent is that the window is locked when feedback is drawn and unlocked when feedback is complete. LockWindowUpdate is not intended for general-purpose suppression of window redraw. Use the WM_SETREDRAW message to disable redrawing of a particular window.
    // If an application with a locked window (or any locked child windows) calls the GetDC, GetDCEx, or BeginPaint function, the called function returns a device context with a visible region that is empty. This will occur until the application unlocks the window by calling LockWindowUpdate, specifying a value of NULL for hWndLock.
    // If an application attempts to draw within a locked window, the system records the extent of the attempted operation in a bounding rectangle. When the window is unlocked, the system invalidates the area within this bounding rectangle, forcing an eventual WM_PAINT message to be sent to the previously locked window and its child windows. If no drawing has occurred while the window updates were locked, no area is invalidated.
    // LockWindowUpdate does not make the specified window invisible and does not clear the WS_VISIBLE style bit. A locked window cannot be moved.

    // https://docs.microsoft.com/en-us/windows/win32/inputdev/wm-appcommand
    // FAPPCOMMAND_KEY   0		User pressed a key.
    // FAPPCOMMAND_MOUSE 0x8000	User dicked a mouse button.

    // DWORD timeGetTime();
    // Retrieves the system time, in milliseconds. The system time is the time elapsed since Windows was started.

    // typedef struct tagMSG {
    //   HWND   hwnd;
    //   UINT   message;
    //   WPARAM wParam;
    //   LPARAM lParam;
    //   DWORD  time;
    //   POINT  pt;
    //   DWORD  lPrivate;
    // } MSG, *PMSG, *NPMSG, *LPMSG;

    timeBeginPeriod(1); // minimum timer resolution, in milliseconds
    auto start = std::chrono::high_resolution_clock::now();

    MSG msg = {};

    while (GetMessage(&msg, NULL, 0, 0)) {
        if (debug) {
            DWORD insend = InSendMessageEx(NULL);
            if (debug > 1) printf("GetMessage hwnd=%p message=%s (%u), wParam=%u, lParam=%lx time=%lu pt=%ld,%ld insend=%lx%s%s%s%s%s\n",
                msg.hwnd, debWin_msg(msg.message), msg.message, msg.wParam, msg.lParam, msg.time, msg.pt.x, msg.pt.y, insend,
                insend & ISMEX_SEND ? " send" : "",
                insend & ISMEX_REPLIED ? " replied" : "",
                insend & ISMEX_NOTIFY ? " notify" : "",
                insend & ISMEX_CALLBACK ? " callback" : "",
                insend & ~15 ? " UNKNOWN" : ""
            );
        }

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
