// g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_asm.cpp -o test_asm && ./test_asm
// g++ -g -O0 -Wall -Wextra -fno-exceptions -fno-rtti test_asm.cpp -o test_asm && ods -Mintel ./test_asm
// g++ -g0 -O0 -Wall -Wextra -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm -fno-pic -fno-inline -fverbose-asm -S test_asm.cpp && v test_asm.s
// (g++ -g0 -O0 -Wall -Wextra -fno-exceptions -fno-rtti -fno-asynchronous-unwind-tables -fno-dwarf2-cfi-asm -fno-pic -fno-inline -fverbose-asm -Wa,-alhsdn=/dev/stdout test_asm.cpp && ./a.out) |v

#include <stdio.h>
#include <sys/mman.h>

extern "C" char func2str[];
char func2str[] = "func2()";

extern "C" int func2(
                         // 32bit	64bit Linux	64bit MS
        int x,		 // [ebp+8]	rdi [rbp-20]	rcx [rsp+8]
        int y		 // [ebp+12]	rsi [rbp-24]	rdx [rsp+16]
                         //		rdx		r8
                         //		rcx		r9
                         //		r8
                         //		r9

                         //             push rbp	sub rsp, (sizeof locals)+32
                         //             mov rbp, rsp
                         //             sub rsp, LSZ+16
) {
    int a = 222;	 // [ebp-4]	[rbp-4]		[rsp]
    int b = 333;	 // [ebp-8]	[rbp-8]		[rsp+4]
    return x+y+a+b+1234; // eax		rax		rax
}

int iv;
const char *p;

extern void asm_main_text()
{
    asm volatile (R"(
        {.intel_syntax noprefix | }

        #call asm_main
        jmp asm_exit

        .section data1, "awx"
        .align 0x1000
asm_start:

        .align 16
        .global t1,t2,t3
t1:	.string	"string here\n"
t2:     .ascii	"ascii here\n"
t3:     .asciz	"asciz here\n"
t_:
        .align 16
b1:	.byte	123
b_:
        .align 2
w1:	.short	234
w2:     .word	235
w3:     .hword  236
w4:     .value	237
w_:
        .align 4
i1:	.int	345
i2:	.long	345
i_:
        .align 8
        .zero	8

        .global func1
        .align 16
func1:				# int func1(char c, int idx) { int a, b; ....; return ax; }
        push ebp
        mov ebp, esp
        sub esp, 8

        # [esp] -> [ebp-8]    | b         |
        #          [ebp-4]    | a         |
        # [ebp] ->            | old_ebp   |
        #          [ebp+4]    | ret-addr  |
        #          [ebp+8]    | c         |
        #          [ebp+12]   | idx       |

        #int 3

        mov al, [ebp+8] # c
        mov ebx, [ebp+12] # idx
        mov [t1+ebx], al
        mov eax, offset t1
        mov [p], eax

        mov ecx, 130000
        mov eax, 1
1:	inc eax
        loop 1b
        mov [iv], eax

        pop eax
        pop ax

        mov esp, ebp
        pop ebp
        ret

        .global asm_main
        .align 16
asm_main:
        push 0
        push 'A'
        call func1
        add esp, 8
        ret

abs0:	mov eax, offset abs1
        mov eax, offset abs2
        mov eax, offset abs2

asm_end:

        .section absolute, "wxa"
        .org 0x100000
abs1:
#	.string "section absolute1"
        .org 0x101100
abs2:
#	.string "section absolute2"
        .org 0x101200
abs3:

.text
asm_exit:
        {.att_syntax noprefix | }
)"
        : /*out*/ //[i] "+m" (i), [ip] "+p" (i), [j] "+m" (j)
        : /*in*/  //[i] "a" (&i), [j] "b" (&j)
        : /*clobber*/ "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory"
    );
}

extern "C" void asm_main();
extern char asm_start[];
extern char asm_end[];

extern char abs1[];
extern char abs2[];

int main()
{
    printf("=== func2(100,200)=%d\n", func2(100, 200));

    //asm_main(); // it doesn't save ebx
    asm volatile ("call asm_main" ::: /*clobber*/ "eax", "ebx", "ecx", "edx", "esi", "edi", "cc", "memory");

    printf("=== p=%s iv=%d\n", p, iv);
    printf("=== asm_start=%p asm_end=%p size=%d\n", asm_start, asm_end, asm_end-asm_start);

    void *ptr = mmap((void*)0x1000000, 4096, PROT_READ|PROT_WRITE|PROT_EXEC, MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED|MAP_LOCKED|MAP_POPULATE, -1, 0);
    printf("=== mmap=%p\n", ptr);
}

/*

https://gcc.gnu.org/onlinedocs/gcc/Machine-Constraints.html#Machine-Constraints
x86 family—config/i386/constraints.md

R	Legacy register—the eight integer registers available on all i386 processors (a, b, c, d, si, di, bp, sp).
q	Any register accessible as rl. In 32-bit mode, a, b, c, and d; in 64-bit mode, any integer register.
Q	Any register accessible as rh: a, b, c, and d.
a	The a register.
b	The b register.
c	The c register.
d	The d register.
S	The si register.
D	The di register.
A	The a and d registers. This class is used for instructions that return double word results in the ax:dx register pair. Single word values will be allocated either in ax or dx.
        For example on i386 the following implements rdtsc:
        unsigned long long rdtsc (void) { unsigned long long tick; __asm__ __volatile__("rdtsc":"=A"(tick)); return tick; }
        This is not correct on x86-64 as it would allocate tick in either ax or dx. You have to use the following variant instead:
        unsigned long long rdtsc (void) { unsigned int tickl, tickh; __asm__ __volatile__("rdtsc":"=a"(tickl),"=d"(tickh)); return ((unsigned long long)tickh << 32)|tickl; }
U	The call-clobbered integer registers.
f	Any 80387 floating-point (stack) register.
t	Top of 80387 floating-point stack (%st(0)).
u	Second from top of 80387 floating-point stack (%st(1)).
y	Any MMX register.
x	Any SSE register.
v	Any EVEX encodable SSE register (%xmm0-%xmm31).
Yz	First SSE register (%xmm0).
I	Integer constant in the range 0 … 31, for 32-bit shifts.
J	Integer constant in the range 0 … 63, for 64-bit shifts.
K	Signed 8-bit integer constant.
L	0xFF or 0xFFFF, for andsi as a zero-extending move.
M	0, 1, 2, or 3 (shifts for the lea instruction).
N	Unsigned 8-bit integer constant (for in and out instructions).
G	Standard 80387 floating point constant.
C	SSE constant zero operand.
e	32-bit signed integer constant, or a symbolic reference known to fit that range (for immediate operands in sign-extending x86-64 instructions).
We	32-bit signed integer constant, or a symbolic reference known to fit that range (for sign-extending conversion operations that require non-VOIDmode immediate operands).
Wz	32-bit unsigned integer constant, or a symbolic reference known to fit that range (for zero-extending conversion operations that require non-VOIDmode immediate operands).
Wd	128-bit integer constant where both the high and low 64-bit word satisfy the e constraint.
Z	32-bit unsigned integer constant, or a symbolic reference known to fit that range (for immediate operands in zero-extending x86-64 instructions).
Tv	VSIB address operand.
Ts	Address operand without segment register.


i386/x86_64 register correspondence:
  rax rcx rdx rbx rsp rbp rsi rdi -   -   -   -   -   -   -   -
  r0  r1  r2  r3  r4  r5  r6  r7  r8  r9 r10 r11 r12 r13 r14 r15

Calling conventions:
  Kernel int 0x80: eax, ebx, ecx, edx, esi, edi, ebp
  Kernel sysenter: eax, edi, esi, edx, rcx, r8, r9
  Kernel syscall: rax, rdi, rsi, rdx, r10, r8, r9
  System V AMD64: rdi, rsi, rdx, rcx, r8, r9 (preserve rbp, rbx, r12-r15; return rdx:rax)
  Microsoft x64: rcx, rdx, r8, r9 (preserve rbx, rbp, rdi, rsi, rsp, r12-r15; return rax)

Platform                System V i386           System V X86_64                 ARM
----------------------- ----------------------- ------------------------------- ----------------
Return Value            eax, edx                rax, rdx                        r0, r1
Parameter Registers     none                    rdi, rsi, rdx, rcx, r8, r9      r0, r1, r2, r3
Additional Parameters   stack (r to l)          stack (r to l)                  stack
Stack Alignment         16-byte at call         8 byte
Scratch Registers       eax, ecx, edx           rax, rdi, rsi, rdx, rcx, r8-r11 r0-r3, r12
Preserved Registers     ebx, esi, edi, ebp, esp rbx, rsp, rbp, r12-r15          r4-r11, r13, r14
Call List               ebp                     rbp

*/
