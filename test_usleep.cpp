// i686-w64-mingw32-g++-win32 -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ test_usleep.cpp -o test_usleep_w.exe -lws2_32 -lwinmm -Wl,-Bstatic -lwinpthread -Wl,-Bdynamic
// i686-w64-mingw32-g++-posix -g -O0 -Wall -Wextra -mconsole -static-libgcc -static-libstdc++ test_usleep.cpp -o test_usleep_p.exe -lws2_32 -lwinmm
// objdump -p test_usleep_w.exe |fgrep -i .dll
// cp -vp test_usleep* /mnt/local/tmp/

#ifdef WIN32
#define _WIN32_WINNT 0x0601 // Windows 7
#include <windows.h>
#endif

#include <stdio.h> // printf()
#include <unistd.h> // usleep()
#include <time.h> // time(), nanosleep()
#include <thread>
#include <chrono>

#ifdef linux
#include <sys/select.h> // select()
#endif

// 150->280, 900->1100, 1000->1200, 1100->1300
// - timeBeginPeriod: 150->0, 900->0, 1000->15777, 1100->15777
// + timeBeginPeriod: 150->0, 900->0, 1000->2029, 1100->2006
int usleep(useconds_t usec);

// 150->280, 900->1100, 1000->1200, 1100->1300
// - timeBeginPeriod: 150->0, 900->0, 1000->15777, 1100->15777
// + timeBeginPeriod: 150->0, 900->0, 1000->1642, 1100->1559
int usleep_nanosleep(useconds_t usec)
{
    /* windows: 1ms resolution */
    struct timespec req = {}, rem = {};
    req.tv_sec = usec / 1000000L;
    req.tv_nsec = (usec % 1000000L) * 1000;
    nanosleep(&req, &rem);
    return 0;
}

// 150->284, 900->1095, 1000->1200, 1100->1298
// - timeBeginPeriod: 150->0, 900->0, 1000->0, 1100->0
// + timeBeginPeriod: 150->0, 900->0, 1000->0, 1100->0
int usleep_select(useconds_t usec)
{
    /* windows: 1ms resolution */
    struct timeval tv;
    tv.tv_sec = usec / 1000000L;
    tv.tv_usec = usec % 1000000L;
    select(0, NULL, NULL, NULL, &tv);
    return 0;
}

#ifdef WIN32
// - timeBeginPeriod: 150->15124, 900->15722, 1000->15735, 1100->15859
// + timeBeginPeriod: 150->1011, 900->1005, 1000->1549, 1100->2020
int usleep_wt(useconds_t usec)
{
    /* windows: 1ms resolution */
    static HANDLE timer = NULL;
    LARGE_INTEGER ft;
    ft.QuadPart = -10LL * usec; // Convert to 100 nanosecond interval, negative value indicates relative time
    if (!timer) timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    //CloseHandle(timer);
    return 0;
}
#endif

#ifdef linux
// 150->283, 900->1100, 1000->1196, 1100->1300
int usleep_th(useconds_t usec)
{
    std::chrono::microseconds d(usec);
    std::this_thread::sleep_for(d);
    return 0;
}
#endif

int res;
//void QueryInterruptTimePrecise(PULONGLONG lpInterruptTimePrecise); // interrupt-time count in system time units of 100 nanoseconds

void test_usleep(const char *name, int (*fn)(useconds_t usec), useconds_t usec)
{
    int ncalls = 2000000 / usec;
#ifdef WIN32
    unsigned long long itime_start, itime_end;
    QueryUnbiasedInterruptTime(&itime_start);
#endif
    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < ncalls; i++) res = fn(usec);
    std::chrono::duration<double, std::micro> ti = std::chrono::high_resolution_clock::now() - start;
#ifdef WIN32
    QueryUnbiasedInterruptTime(&itime_end);
    printf("%s: %d -> %.2f usec/call\n", name, usec, 0.1 * (itime_end-itime_start) / ncalls);
#endif
    //printf("%s(%d): %d calls, %.2f (itime %.2f) usec/call\n", name, usec, ncalls, ti.count() / ncalls,
    //    (itime_end - itime_start) / 10.0 / ncalls);
    printf("%s: %d -> %.2f usec/call\n", name, usec, ti.count() / ncalls);
}

#define test_fn(fn) do { \
    test_usleep(#fn, fn, 150); \
    test_usleep(#fn, fn, 900); \
    test_usleep(#fn, fn, 1000); \
    test_usleep(#fn, fn, 1100); \
    test_usleep(#fn, fn, 1900); \
    test_usleep(#fn, fn, 2000); \
} while (0)

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;
#ifdef WIN32
    // Initialize Winsock
    WSADATA wsaData;
    int res = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (res != 0) {
        printf("WSAStartup failed: %d\n", res);
        return 1;
    }

    if (argc < 2) timeBeginPeriod(1); // minimum timer resolution, in milliseconds
#endif

    test_fn(usleep);
    test_fn(usleep_nanosleep);
    test_fn(usleep_select);
#ifdef WIN32
    test_fn(usleep_wt);
#endif
#ifdef linux
    test_fn(usleep_th);
#endif

#ifdef WIN32
    if (argc < 2) timeEndPeriod(1); // match each call to timeBeginPeriod with a call to timeEndPeriod, specifying the same minimum resolution
#endif
}
