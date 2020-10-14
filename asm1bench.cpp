#include "asm1.cpp"

#ifdef MAIN
// g++ -fno-pic -fno-pie -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -DMAIN asm1s.S asm1.cpp && ./a.out
// i686-w64-mingw32-g++ -fno-pic -fno-pie -mconsole -static-libgcc -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti -DMAIN asm1s.S asm1.cpp && wine ./a.exe

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

uint64_t rdtsc(void)
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
    printf("eloop = %.2f us/1k\n", 1.0/1730/100 * eloop);

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        strlen_test_asm(100);
    }
    end = rdtsc();
    uint64_t strlen_test_asmT = end - beg;
    printf("strlen_test_asm x 100 = %.2f us/1k\n", 1.0/1730/100 * strlen_test_asmT);

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        PutCircleAsm(100, 100, 0xff1133, 44);
    }
    end = rdtsc();
    uint64_t PutCircleAsmT = end - beg;
    printf("PutCircleAsm = %.2f us/1k\n", 1.0/1730/100 * PutCircleAsmT);

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        PutCircleNew(100, 100, 0xff1133, 44);
    }
    end = rdtsc();
    uint64_t PutCircleNewT = end - beg;
    printf("PutCircleNew = %.2f us/1k\n", 1.0/1730/100 * PutCircleNewT);

    beg = rdtsc();
    for (int i = 0; i < 100000; i++) {
        PutTexturedCircle(100, 100, 44, SolidBlackTex, NULL);
    }
    end = rdtsc();
    uint64_t PutTexturedCircleT = end - beg;
    printf("PutTexturedCircle = %.2f us/1k\n", 1.0/1730/100 * PutTexturedCircleT);
}
#endif
