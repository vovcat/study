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
t1:	.string "string here"
t2:     .ascii "ascii here\n"
t3:     .asciz "asciz here\n"
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

        mov ecx, 0x0fffffff
        mov eax, 1
1:	inc eax
        loop 1b
        mov [iv], eax

        mov esp, ebp
        pop ebp
        ret

add:
        add  [eax],al		# 00 00
        add  [ecx],al		# 00 01
        add  [edx],al		# 00 02
        add  [ebx],al		# 00 03
        add  [eax+eax*1],al	# 00 04 00
        add  [0x12345678],al	# 00 05 78 56 34 12
        add  [esi],al		# 00 06
        add  [edi],al		# 00 07

        add  [eax],cl		# 00 08
        add  [ecx],cl		# 00 09
        add  [edx],cl		# 00 0a
        add  [ebx],cl		# 00 0b
        add  [eax+eax*1],cl	# 00 0c 00
        add  [0x12345678],cl	# 00 0d 78 56 34 12
        add  [esi],cl		# 00 0e
        add  [edi],cl		# 00 0f

        add  [eax],dl		# 00 10
        add  [ecx],dl		# 00 11
        add  [edx],dl		# 00 12
        add  [ebx],dl		# 00 13
        add  [eax+eax*1],dl	# 00 14 00
        add  [0x12345678],dl	# 00 15 78 56 34 12
        add  [esi],dl		# 00 16
        add  [edi],dl		# 00 17

        add  [eax],bl		# 00 18
        add  [ecx],bl		# 00 19
        add  [edx],bl		# 00 1a
        add  [ebx],bl		# 00 1b
        add  [eax+eax*1],bl	# 00 1c 00
        add  [0x12345678],bl	# 00 1d 78 56 34 12
        add  [esi],bl		# 00 1e
        add  [edi],bl		# 00 1f

        add  [eax],ah		# 00 20
        add  [ecx],ah		# 00 21
        add  [edx],ah		# 00 22
        add  [ebx],ah		# 00 23
        add  [eax+eax*1],ah	# 00 24 00
        add  [0x12345678],ah	# 00 25 78 56 34 12
        add  [esi],ah		# 00 26
        add  [edi],ah		# 00 27

        add  [eax],ch		# 00 28
        add  [ecx],ch		# 00 29
        add  [edx],ch		# 00 2a
        add  [ebx],ch		# 00 2b
        add  [eax+eax*1],ch	# 00 2c 00
        add  [0x12345678],ch	# 00 2d 78 56 34 12
        add  [esi],ch		# 00 2e
        add  [edi],ch		# 00 2f

        add  [eax],dh		# 00 30
        add  [ecx],dh		# 00 31
        add  [edx],dh		# 00 32
        add  [ebx],dh		# 00 33
        add  [eax+eax*1],dh	# 00 34 00
        add  [0x12345678],dh	# 00 35 78 56 34 12
        add  [esi],dh		# 00 36
        add  [edi],dh		# 00 37

        add  [eax],bh		# 00 38
        add  [ecx],bh		# 00 39
        add  [edx],bh		# 00 3a
        add  [ebx],bh		# 00 3b
        add  [eax+eax*1],bh	# 00 3c 00
        add  [0x12345678],bh	# 00 3d 78 56 34 12
        add  [esi],bh		# 00 3e
        add  [edi],bh		# 00 3f

        add  [eax-1],al		# 00 40 ff
        add  [ecx-1],al		# 00 41 ff
        add  [edx-1],al		# 00 42 ff
        add  [ebx-1],al		# 00 43 ff
        add  [eax+eax*1-0x1],al	# 00 44 00 ff
        add  [ebp-0x1],al	# 00 45 ff
        add  [esi-0x1],al	# 00 46 ff
        add  [edi-0x1],al	# 00 47 ff

        add  [eax-1],cl		# 00 48 ff
        add  [ecx-1],cl		# 00 49 ff
        add  [edx-1],cl		# 00 4a ff
        add  [ebx-1],cl		# 00 4b ff
        add  [eax+eax*1-0x1],cl	# 00 4c 00 ff
        add  [ebp-0x1],cl	# 00 4d ff
        add  [esi-0x1],cl	# 00 4e ff
        add  [edi-0x1],cl	# 00 4f ff

        add  [eax-1],dl		# 00 50 ff
        add  [ecx-1],dl		# 00 51 ff
        add  [edx-1],dl		# 00 52 ff
        add  [ebx-1],dl		# 00 53 ff
        add  [eax+eax*1-0x1],dl	# 00 54 00 ff
        add  [ebp-0x1],dl	# 00 55 ff
        add  [esi-0x1],dl	# 00 56 ff
        add  [edi-0x1],dl	# 00 57 ff

        add  [eax-1],bl		# 00 58 ff
        add  [ecx-1],bl		# 00 59 ff
        add  [edx-1],bl		# 00 5a ff
        add  [ebx-1],bl		# 00 5b ff
        add  [eax+eax*1-0x1],bl	# 00 5c 00 ff
        add  [ebp-0x1],bl	# 00 5d ff
        add  [esi-0x1],bl	# 00 5e ff
        add  [edi-0x1],bl	# 00 5f ff

        add  [eax-1],ah		# 00 60 ff
        add  [ecx-1],ah		# 00 61 ff
        add  [edx-1],ah		# 00 62 ff
        add  [ebx-1],ah		# 00 63 ff
        add  [eax+eax*1-0x1],ah	# 00 64 00 ff
        add  [ebp-0x1],ah	# 00 65 ff
        add  [esi-0x1],ah	# 00 66 ff
        add  [edi-0x1],ah	# 00 67 ff

        add  [eax-1],ch		# 00 68 ff
        add  [ecx-1],ch		# 00 69 ff
        add  [edx-1],ch		# 00 6a ff
        add  [ebx-1],ch		# 00 6b ff
        add  [eax+eax*1-0x1],ch	# 00 6c 00 ff
        add  [ebp-0x1],ch	# 00 6d ff
        add  [esi-0x1],ch	# 00 6e ff
        add  [edi-0x1],ch	# 00 6f ff

        add  [eax-1],dh		# 00 70 ff
        add  [ecx-1],dh		# 00 71 ff
        add  [edx-1],dh		# 00 72 ff
        add  [ebx-1],dh		# 00 73 ff
        add  [eax+eax*1-0x1],dh	# 00 74 00 ff
        add  [ebp-0x1],dh	# 00 75 ff
        add  [esi-0x1],dh	# 00 76 ff
        add  [edi-0x1],dh	# 00 77 ff

        add  [eax-1],bh		# 00 78 ff
        add  [ecx-1],bh		# 00 79 ff
        add  [edx-1],bh		# 00 7a ff
        add  [ebx-1],bh		# 00 7b ff
        add  [eax+eax*1-0x1],bh	# 00 7c 00 ff
        add  [ebp-0x1],bh	# 00 7d ff
        add  [esi-0x1],bh	# 00 7e ff
        add  [edi-0x1],bh	# 00 7f ff

        add  [eax+0x12345678],al	 # 00 80 .. .. .. ..
        add  [ecx+0x12345678],al	 # 00 81 .. .. .. ..
        add  [edx+0x12345678],al	 # 00 82 .. .. .. ..
        add  [ebx+0x12345678],al	 # 00 83 .. .. .. ..
        add  [eax+eax*1+0x12345678],al	 # 00 84 00 .. .. .. ..
        add  [ebp+0x12345678],al	 # 00 85 .. .. .. ..
        add  [esi+0x12345678],al	 # 00 86 .. .. .. ..
        add  [edi+0x12345678],al	 # 00 87 .. .. .. ..

        add  [eax+0x12345678],cl	 # 00 88 .. .. .. ..
        add  [ecx+0x12345678],cl	 # 00 89 .. .. .. ..
        add  [edx+0x12345678],cl	 # 00 8a .. .. .. ..
        add  [ebx+0x12345678],cl	 # 00 8b .. .. .. ..
        add  [eax+eax*1+0x12345678],cl	 # 00 8c 00 .. .. .. ..
        add  [ebp+0x12345678],cl	 # 00 8d .. .. .. ..
        add  [esi+0x12345678],cl	 # 00 8e .. .. .. ..
        add  [edi+0x12345678],cl	 # 00 8f .. .. .. ..

        add  [eax+0x12345678],dl	 # 00 90 .. .. .. ..
        add  [ecx+0x12345678],dl	 # 00 91 .. .. .. ..
        add  [edx+0x12345678],dl	 # 00 92 .. .. .. ..
        add  [ebx+0x12345678],dl	 # 00 93 .. .. .. ..
        add  [eax+eax*1+0x12345678],dl	 # 00 94 00 .. .. .. ..
        add  [ebp+0x12345678],dl	 # 00 95 .. .. .. ..
        add  [esi+0x12345678],dl	 # 00 96 .. .. .. ..
        add  [edi+0x12345678],dl	 # 00 97 .. .. .. ..

        add  [eax+0x12345678],bl	 # 00 98 .. .. .. ..
        add  [ecx+0x12345678],bl	 # 00 99 .. .. .. ..
        add  [edx+0x12345678],bl	 # 00 9a .. .. .. ..
        add  [ebx+0x12345678],bl	 # 00 9b .. .. .. ..
        add  [eax+eax*1+0x12345678],bl	 # 00 9c 00 .. .. .. ..
        add  [ebp+0x12345678],bl	 # 00 9d .. .. .. ..
        add  [esi+0x12345678],bl	 # 00 9e .. .. .. ..
        add  [edi+0x12345678],bl	 # 00 9f .. .. .. ..

        add  [eax+0x12345678],ah	 # 00 a0 .. .. .. ..
        add  [ecx+0x12345678],ah	 # 00 a1 .. .. .. ..
        add  [edx+0x12345678],ah	 # 00 a2 .. .. .. ..
        add  [ebx+0x12345678],ah	 # 00 a3 .. .. .. ..
        add  [eax+eax*1+0x12345678],ah	 # 00 a4 00 .. .. .. ..
        add  [ebp+0x12345678],ah	 # 00 a5 .. .. .. ..
        add  [esi+0x12345678],ah	 # 00 a6 .. .. .. ..
        add  [edi+0x12345678],ah	 # 00 a7 .. .. .. ..

        add  [eax+0x12345678],ch	 # 00 a8 .. .. .. ..
        add  [ecx+0x12345678],ch	 # 00 a9 .. .. .. ..
        add  [edx+0x12345678],ch	 # 00 aa .. .. .. ..
        add  [ebx+0x12345678],ch	 # 00 ab .. .. .. ..
        add  [eax+eax*1+0x12345678],ch	 # 00 ac 00 .. .. .. ..
        add  [ebp+0x12345678],ch	 # 00 ad .. .. .. ..
        add  [esi+0x12345678],ch	 # 00 ae .. .. .. ..
        add  [edi+0x12345678],ch	 # 00 af .. .. .. ..

        add  [eax+0x12345678],dh	 # 00 b0 .. .. .. ..
        add  [ecx+0x12345678],dh	 # 00 b1 .. .. .. ..
        add  [edx+0x12345678],dh	 # 00 b2 .. .. .. ..
        add  [ebx+0x12345678],dh	 # 00 b3 .. .. .. ..
        add  [eax+eax*1+0x12345678],dh	 # 00 b4 00 .. .. .. ..
        add  [ebp+0x12345678],dh	 # 00 b5 .. .. .. ..
        add  [esi+0x12345678],dh	 # 00 b6 .. .. .. ..
        add  [edi+0x12345678],dh	 # 00 b7 .. .. .. ..

        add  [eax+0x12345678],bh	 # 00 b8 .. .. .. ..
        add  [ecx+0x12345678],bh	 # 00 b9 .. .. .. ..
        add  [edx+0x12345678],bh	 # 00 ba .. .. .. ..
        add  [ebx+0x12345678],bh	 # 00 bb .. .. .. ..
        add  [eax+eax*1+0x12345678],bh	 # 00 bc 00 .. .. .. ..
        add  [ebp+0x12345678],bh	 # 00 bd .. .. .. ..
        add  [esi+0x12345678],bh	 # 00 be .. .. .. ..
        add  [edi+0x12345678],bh	 # 00 bf .. .. .. ..

        add al,al                                # 00 c0
        add cl,al                                # 00 c1
        add dl,al                                # 00 c2
        add bl,al                                # 00 c3
        add ah,al                                # 00 c4
        add ch,al                                # 00 c5
        add dh,al                                # 00 c6
        add bh,al                                # 00 c7

        add al,cl                                # 00 c8
        add cl,cl                                # 00 c9
        add dl,cl                                # 00 ca
        add bl,cl                                # 00 cb
        add ah,cl                                # 00 cc
        add ch,cl                                # 00 cd
        add dh,cl                                # 00 ce
        add bh,cl                                # 00 cf

        add al,dl                                # 00 d0
        add cl,dl                                # 00 d1
        add dl,dl                                # 00 d2
        add bl,dl                                # 00 d3
        add ah,dl                                # 00 d4
        add ch,dl                                # 00 d5
        add dh,dl                                # 00 d6
        add bh,dl                                # 00 d7

        add al,dl                                # 00 d8
        add cl,dl                                # 00 d9
        add dl,dl                                # 00 da
        add bl,dl                                # 00 db
        add ah,dl                                # 00 dc
        add ch,dl                                # 00 dd
        add dh,dl                                # 00 de
        add bh,dl                                # 00 df

        add al,ah                                # 00 e0
        add cl,ah                                # 00 e1
        add dl,ah                                # 00 e2
        add bl,ah                                # 00 e3
        add ah,ah                                # 00 e4
        add ch,ah                                # 00 e5
        add dh,ah                                # 00 e6
        add bh,ah                                # 00 e7

        add al,ch                                # 00 e8
        add cl,ch                                # 00 e9
        add dl,ch                                # 00 ea
        add bl,ch                                # 00 eb
        add ah,ch                                # 00 ec
        add ch,ch                                # 00 ed
        add dh,ch                                # 00 ee
        add bh,ch                                # 00 ef

        add al,dh                                # 00 f0
        add cl,dh                                # 00 f1
        add dl,dh                                # 00 f2
        add bl,dh                                # 00 f3
        add ah,dh                                # 00 f4
        add ch,dh                                # 00 f5
        add dh,dh                                # 00 f6
        add bh,dh                                # 00 f7

        add al,bh                                # 00 f8
        add cl,bh                                # 00 f9
        add dl,bh                                # 00 fa
        add bl,bh                                # 00 fb
        add ah,bh                                # 00 fc
        add ch,bh                                # 00 fd
        add dh,bh                                # 00 fe
        add bh,bh                                # 00 ff

        add  [eax],eax		         # 01 00
        add  [ecx],eax		         # 01 01
        add  [edx],eax		         # 01 02
        add  [ebx],eax		         # 01 03
        add  [eax+eax*1],eax	         # 01 04 00
        add  [0x12345678],eax	         # 01 05 78 56 34 12
        add  [esi],eax		         # 01 06
        add  [edi],eax		         # 01 07

        add  [eax],ecx		         # 01 08
        add  [ecx],ecx		         # 01 09
        add  [edx],ecx		         # 01 0a
        add  [ebx],ecx		         # 01 0b
        add  [eax+eax*1],ecx	         # 01 0c 00
        add  [0x12345678],ecx	         # 01 0d 78 56 34 12
        add  [esi],ecx		         # 01 0e
        add  [edi],ecx		         # 01 0f

        add  [eax],edx		         # 01 10
        add  [ecx],edx		         # 01 11
        add  [edx],edx		         # 01 12
        add  [ebx],edx		         # 01 13
        add  [eax+eax*1],edx	         # 01 14 00
        add  [0x12345678],edx	         # 01 15 78 56 34 12
        add  [esi],edx		         # 01 16
        add  [edi],edx		         # 01 17

        add  [eax],ebx		         # 01 18
        add  [ecx],ebx		         # 01 19
        add  [edx],ebx		         # 01 1a
        add  [ebx],ebx		         # 01 1b
        add  [eax+eax*1],ebx	         # 01 1c 00
        add  [0x12345678],ebx	         # 01 1d 78 56 34 12
        add  [esi],ebx		         # 01 1e
        add  [edi],ebx		         # 01 1f

        add  [eax],esp		         # 01 20
        add  [ecx],esp		         # 01 21
        add  [edx],esp		         # 01 22
        add  [ebx],esp		         # 01 23
        add  [eax+eax*1],esp	         # 01 24 00
        add  [0x12345678],esp	         # 01 25 78 56 34 12
        add  [esi],esp		         # 01 26
        add  [edi],esp		         # 01 27

        add  [eax],ebp		         # 01 28
        add  [ecx],ebp		         # 01 29
        add  [edx],ebp		         # 01 2a
        add  [ebx],ebp		         # 01 2b
        add  [eax+eax*1],ebp	         # 01 2c 00
        add  [0x12345678],ebp	         # 01 2d 78 56 34 12
        add  [esi],ebp		         # 01 2e
        add  [edi],ebp		         # 01 2f

        add  [eax],esi		         # 01 30
        add  [ecx],esi		         # 01 31
        add  [edx],esi		         # 01 32
        add  [ebx],esi		         # 01 33
        add  [eax+eax*1],esi	         # 01 34 00
        add  [0x12345678],esi	         # 01 35 78 56 34 12
        add  [esi],esi		         # 01 36
        add  [edi],esi		         # 01 37

        add  [eax],edi		         # 01 38
        add  [ecx],edi		         # 01 39
        add  [edx],edi		         # 01 3a
        add  [ebx],edi		         # 01 3b
        add  [eax+eax*1],edi	         # 01 3c 00
        add  [0x12345678],edi	         # 01 3d 78 56 34 12
        add  [esi],edi		         # 01 3e
        add  [edi],edi		         # 01 3f

        add  [eax-0x1],eax              # 01 40 ff
        add  [ecx-0x1],eax              # 01 41 ff
        add  [edx-0x1],eax              # 01 42 ff
        add  [ebx-0x1],eax              # 01 43 ff
        add  [eax+eax*1-0x1],eax        # 01 44 00 ff
        add  [ebp-0x1],eax              # 01 45 ff
        add  [esi-0x1],eax              # 01 46 ff
        add  [edi-0x1],eax              # 01 47 ff

        add  [eax-0x1],ecx              # 01 48 ff
        add  [ecx-0x1],ecx              # 01 49 ff
        add  [edx-0x1],ecx              # 01 4a ff
        add  [ebx-0x1],ecx              # 01 4b ff
        add  [eax+eax*1-0x1],ecx        # 01 4c 00 ff
        add  [ebp-0x1],ecx              # 01 4d ff
        add  [esi-0x1],ecx              # 01 4e ff
        add  [edi-0x1],ecx              # 01 4f ff

        add  [eax-0x1],edx              # 01 50 ff
        add  [ecx-0x1],edx              # 01 51 ff
        add  [edx-0x1],edx              # 01 52 ff
        add  [ebx-0x1],edx              # 01 53 ff
        add  [eax+eax*1-0x1],edx        # 01 54 00 ff
        add  [ebp-0x1],edx              # 01 55 ff
        add  [esi-0x1],edx              # 01 56 ff
        add  [edi-0x1],edx              # 01 57 ff

        add  [eax-0x1],ebx              # 01 58 ff
        add  [ecx-0x1],ebx              # 01 59 ff
        add  [edx-0x1],ebx              # 01 5a ff
        add  [ebx-0x1],ebx              # 01 5b ff
        add  [eax+eax*1-0x1],ebx        # 01 5c 00 ff
        add  [ebp-0x1],ebx              # 01 5d ff
        add  [esi-0x1],ebx              # 01 5e ff
        add  [edi-0x1],ebx              # 01 5f ff

        add  [eax-0x1],esp              # 01 60 ff
        add  [ecx-0x1],esp              # 01 61 ff
        add  [edx-0x1],esp              # 01 62 ff
        add  [ebx-0x1],esp              # 01 63 ff
        add  [eax+eax*1-0x1],esp        # 01 64 00 ff
        add  [ebp-0x1],esp              # 01 65 ff
        add  [esi-0x1],esp              # 01 66 ff
        add  [edi-0x1],esp              # 01 67 ff

        add  [eax-0x1],ebp              # 01 68 ff
        add  [ecx-0x1],ebp              # 01 69 ff
        add  [edx-0x1],ebp              # 01 6a ff
        add  [ebx-0x1],ebp              # 01 6b ff
        add  [eax+eax*1-0x1],ebp        # 01 6c 00 ff
        add  [ebp-0x1],ebp              # 01 6d ff
        add  [esi-0x1],ebp              # 01 6e ff
        add  [edi-0x1],ebp              # 01 6f ff

        add  [eax-0x1],esp              # 01 60 ff
        add  [ecx-0x1],esp              # 01 61 ff
        add  [edx-0x1],esp              # 01 62 ff
        add  [ebx-0x1],esp              # 01 63 ff
        add  [eax+eax*1-0x1],esp        # 01 64 00 ff
        add  [ebp-0x1],esp              # 01 65 ff
        add  [esi-0x1],esp              # 01 66 ff
        add  [edi-0x1],esp              # 01 67 ff

        add  [eax-0x1],ebp              # 01 68 ff
        add  [ecx-0x1],ebp              # 01 69 ff
        add  [edx-0x1],ebp              # 01 6a ff
        add  [ebx-0x1],ebp              # 01 6b ff
        add  [eax+eax*1-0x1],ebp        # 01 6c 00 ff
        add  [ebp-0x1],ebp              # 01 6d ff
        add  [esi-0x1],ebp              # 01 6e ff
        add  [edi-0x1],ebp              # 01 6f ff

        add  [eax-0x1],esi              # 01 70 ff
        add  [ecx-0x1],esi              # 01 71 ff
        add  [edx-0x1],esi              # 01 72 ff
        add  [ebx-0x1],esi              # 01 73 ff
        add  [eax+eax*1-0x1],esi        # 01 74 00 ff
        add  [ebp-0x1],esi              # 01 75 ff
        add  [esi-0x1],esi              # 01 76 ff
        add  [edi-0x1],esi              # 01 77 ff

        add  [eax-0x1],edi              # 01 78 ff
        add  [ecx-0x1],edi              # 01 79 ff
        add  [edx-0x1],edi              # 01 7a ff
        add  [ebx-0x1],edi              # 01 7b ff
        add  [eax+eax*1-0x1],edi        # 01 7c 00 ff
        add  [ebp-0x1],edi              # 01 7d ff
        add  [esi-0x1],edi              # 01 7e ff
        add  [edi-0x1],edi              # 01 7f ff

        add  [eax+0x12345678],eax       # 01 80 78 56 34 12
        add  [ecx+0x12345678],eax       # 01 81 78 56 34 12
        add  [edx+0x12345678],eax       # 01 82 78 56 34 12
        add  [ebx+0x12345678],eax       # 01 83 78 56 34 12
        add  [eax+eax*1+0x12345678],eax # 01 84 00 78 56 34 12
        add  [ebp+0x12345678],eax       # 01 85 78 56 34 12
        add  [esi+0x12345678],eax       # 01 86 78 56 34 12
        add  [edi+0x12345678],eax       # 01 87 78 56 34 12

        add  [eax+0x12345678],ecx       # 01 88 78 56 34 12
        add  [ecx+0x12345678],ecx       # 01 89 78 56 34 12
        add  [edx+0x12345678],ecx       # 01 8a 78 56 34 12
        add  [ebx+0x12345678],ecx       # 01 8b 78 56 34 12
        add  [eax+eax*1+0x12345678],ecx # 01 8c 00 78 56 34 12
        add  [ebp+0x12345678],ecx       # 01 8d 78 56 34 12
        add  [esi+0x12345678],ecx       # 01 8e 78 56 34 12
        add  [edi+0x12345678],ecx       # 01 8f 78 56 34 12

        add  [eax+0x12345678],edx       # 01 90 78 56 34 12
        add  [ecx+0x12345678],edx       # 01 91 78 56 34 12
        add  [edx+0x12345678],edx       # 01 92 78 56 34 12
        add  [ebx+0x12345678],edx       # 01 93 78 56 34 12
        add  [eax+eax*1+0x12345678],edx # 01 94 00 78 56 34 12
        add  [ebp+0x12345678],edx       # 01 95 78 56 34 12
        add  [esi+0x12345678],edx       # 01 96 78 56 34 12
        add  [edi+0x12345678],edx       # 01 97 78 56 34 12

        add  [eax+0x12345678],ebx       # 01 98 78 56 34 12
        add  [ecx+0x12345678],ebx       # 01 99 78 56 34 12
        add  [edx+0x12345678],ebx       # 01 9a 78 56 34 12
        add  [ebx+0x12345678],ebx       # 01 9b 78 56 34 12
        add  [eax+eax*1+0x12345678],ebx # 01 9c 00 78 56 34 12
        add  [ebp+0x12345678],ebx       # 01 9d 78 56 34 12
        add  [esi+0x12345678],ebx       # 01 9e 78 56 34 12
        add  [edi+0x12345678],ebx       # 01 9f 78 56 34 12

        add  [eax+0x12345678],esp       # 01 a0 78 56 34 12
        add  [ecx+0x12345678],esp       # 01 a1 78 56 34 12
        add  [edx+0x12345678],esp       # 01 a2 78 56 34 12
        add  [ebx+0x12345678],esp       # 01 a3 78 56 34 12
        add  [eax+eax*1+0x12345678],esp # 01 a4 00 78 56 34 12
        add  [ebp+0x12345678],esp       # 01 a5 78 56 34 12
        add  [esi+0x12345678],esp       # 01 a6 78 56 34 12
        add  [edi+0x12345678],esp       # 01 a7 78 56 34 12

        add  [eax+0x12345678],ebp       # 01 a8 78 56 34 12
        add  [ecx+0x12345678],ebp       # 01 a9 78 56 34 12
        add  [edx+0x12345678],ebp       # 01 aa 78 56 34 12
        add  [ebx+0x12345678],ebp       # 01 ab 78 56 34 12
        add  [eax+eax*1+0x12345678],ebp # 01 ac 00 78 56 34 12
        add  [ebp+0x12345678],ebp       # 01 ad 78 56 34 12
        add  [esi+0x12345678],ebp       # 01 ae 78 56 34 12
        add  [edi+0x12345678],ebp       # 01 af 78 56 34 12

        add  [eax+0x12345678],esi       # 01 b0 78 56 34 12
        add  [ecx+0x12345678],esi       # 01 b1 78 56 34 12
        add  [edx+0x12345678],esi       # 01 b2 78 56 34 12
        add  [ebx+0x12345678],esi       # 01 b3 78 56 34 12
        add  [eax+eax*1+0x12345678],esi # 01 b4 00 78 56 34 12
        add  [ebp+0x12345678],esi       # 01 b5 78 56 34 12
        add  [esi+0x12345678],esi       # 01 b6 78 56 34 12
        add  [edi+0x12345678],esi       # 01 b7 78 56 34 12

        add  [eax+0x12345678],edi       # 01 b8 78 56 34 12
        add  [ecx+0x12345678],edi       # 01 b9 78 56 34 12
        add  [edx+0x12345678],edi       # 01 ba 78 56 34 12
        add  [ebx+0x12345678],edi       # 01 bb 78 56 34 12
        add  [eax+eax*1+0x12345678],edi # 01 bc 00 78 56 34 12
        add  [ebp+0x12345678],edi       # 01 bd 78 56 34 12
        add  [esi+0x12345678],edi       # 01 be 78 56 34 12
        add  [edi+0x12345678],edi       # 01 bf 78 56 34 12

        add eax,eax                              # 01 c0
        add ecx,eax                              # 01 c1
        add edx,eax                              # 01 c2
        add ebx,eax                              # 01 c3
        add esp,eax                              # 01 c4
        add ebp,eax                              # 01 c5
        add esi,eax                              # 01 c6
        add edi,eax                              # 01 c7

        add eax,ecx                              # 01 c8
        add ecx,ecx                              # 01 c9
        add edx,ecx                              # 01 ca
        add ebx,ecx                              # 01 cb
        add esp,ecx                              # 01 cc
        add ebp,ecx                              # 01 cd
        add esi,ecx                              # 01 ce
        add edi,ecx                              # 01 cf

        add eax,edx                              # 01 d0
        add ecx,edx                              # 01 d1
        add edx,edx                              # 01 d2
        add ebx,edx                              # 01 d3
        add esp,edx                              # 01 d4
        add ebp,edx                              # 01 d5
        add esi,edx                              # 01 d6
        add edi,edx                              # 01 d7

        add eax,ebx                              # 01 d8
        add ecx,ebx                              # 01 d9
        add edx,ebx                              # 01 da
        add ebx,ebx                              # 01 db
        add esp,ebx                              # 01 dc
        add ebp,ebx                              # 01 dd
        add esi,ebx                              # 01 de
        add edi,ebx                              # 01 df

        add eax,esp                              # 01 e0
        add ecx,esp                              # 01 e1
        add edx,esp                              # 01 e2
        add ebx,esp                              # 01 e3
        add esp,esp                              # 01 e4
        add ebp,esp                              # 01 e5
        add esi,esp                              # 01 e6
        add edi,esp                              # 01 e7

        add eax,ebp                              # 01 e8
        add ecx,ebp                              # 01 e9
        add edx,ebp                              # 01 ea
        add ebx,ebp                              # 01 eb
        add esp,ebp                              # 01 ec
        add ebp,ebp                              # 01 ed
        add esi,ebp                              # 01 ee
        add edi,ebp                              # 01 ef

        add eax,esi                              # 01 f0
        add ecx,esi                              # 01 f1
        add edx,esi                              # 01 f2
        add ebx,esi                              # 01 f3
        add esp,esi                              # 01 f4
        add ebp,esi                              # 01 f5
        add esi,esi                              # 01 f6
        add edi,esi                              # 01 f7

        add eax,edi                              # 01 f8
        add ecx,edi                              # 01 f9
        add edx,edi                              # 01 fa
        add ebx,edi                              # 01 fb
        add esp,edi                              # 01 fc
        add ebp,edi                              # 01 fd
        add esi,edi                              # 01 fe
        add edi,edi                              # 01 ff
/*
*/
        add al, [eax]		# 02 00
        add al, [ecx]		# 02 01
        add al, [edx]		# 02 02
        add al, [ebx]		# 02 03
        add al, [eax+eax*1]	# 02 04 00
        add al, [0x12345678]	# 02 05 78 56 34 12
        add al, [esi]		# 02 06
        add al, [edi]		# 02 07

        add cl, [eax]		# 02 08
        add cl, [ecx]		# 02 09
        add cl, [edx]		# 02 0a
        add cl, [ebx]		# 02 0b
        add cl, [eax+eax*1]	# 02 0c 00
        add cl, [0x12345678]	# 02 0d 78 56 34 12
        add cl, [esi]		# 02 0e
        add cl, [edi]		# 02 0f

        add dl, [eax]		# 02 10
        add dl, [ecx]		# 02 11
        add dl, [edx]		# 02 12
        add dl, [ebx]		# 02 13
        add dl, [eax+eax*1]	# 02 14 00
        add dl, [0x12345678]	# 02 15 78 56 34 12
        add dl, [esi]		# 02 16
        add dl, [edi]		# 02 17

        add bl, [eax]		# 02 18
        add bl, [ecx]		# 02 19
        add bl, [edx]		# 02 1a
        add bl, [ebx]		# 02 1b
        add bl, [eax+eax*1]	# 02 1c 00
        add bl, [0x12345678]	# 02 1d 78 56 34 12
        add bl, [esi]		# 02 1e
        add bl, [edi]		# 02 1f

        add ah, [eax]		# 02 20
        add ah, [ecx]		# 02 21
        add ah, [edx]		# 02 22
        add ah, [ebx]		# 02 23
        add ah, [eax+eax*1]	# 02 24 00
        add ah, [0x12345678]	# 02 25 78 56 34 12
        add ah, [esi]		# 02 26
        add ah, [edi]		# 02 27

        add ch, [eax]		# 02 28
        add ch, [ecx]		# 02 29
        add ch, [edx]		# 02 2a
        add ch, [ebx]		# 02 2b
        add ch, [eax+eax*1]	# 02 2c 00
        add ch, [0x12345678]	# 02 2d 78 56 34 12
        add ch, [esi]		# 02 2e
        add ch, [edi]		# 02 2f

        add dh, [eax]		# 02 30
        add dh, [ecx]		# 02 31
        add dh, [edx]		# 02 32
        add dh, [ebx]		# 02 33
        add dh, [eax+eax*1]	# 02 34 00
        add dh, [0x12345678]	# 02 35 78 56 34 12
        add dh, [esi]		# 02 36
        add dh, [edi]		# 02 37

        add bh, [eax]		# 02 38
        add bh, [ecx]		# 02 39
        add bh, [edx]		# 02 3a
        add bh, [ebx]		# 02 3b
        add bh, [eax+eax*1]	# 02 3c 00
        add bh, [0x12345678]	# 02 3d 78 56 34 12
        add bh, [esi]		# 02 3e
        add bh, [edi]		# 02 3f

        add al, [eax-1]		# 02 40 ff
        add al, [ecx-1]		# 02 41 ff
        add al, [edx-1]		# 02 42 ff
        add al, [ebx-1]		# 02 43 ff
        add al, [eax+eax*1-0x1]	# 02 44 00 ff
        add al, [ebp-0x1]	# 02 45 ff
        add al, [esi-0x1]	# 02 46 ff
        add al, [edi-0x1]	# 02 47 ff

        add cl, [eax-1]		# 02 48 ff
        add cl, [ecx-1]		# 02 49 ff
        add cl, [edx-1]		# 02 4a ff
        add cl, [ebx-1]		# 02 4b ff
        add cl, [eax+eax*1-0x1]	# 02 4c 00 ff
        add cl, [ebp-0x1]	# 02 4d ff
        add cl, [esi-0x1]	# 02 4e ff
        add cl, [edi-0x1]	# 02 4f ff

        add dl, [eax-1]		# 02 50 ff
        add dl, [ecx-1]		# 02 51 ff
        add dl, [edx-1]		# 02 52 ff
        add dl, [ebx-1]		# 02 53 ff
        add dl, [eax+eax*1-0x1]	# 02 54 00 ff
        add dl, [ebp-0x1]	# 02 55 ff
        add dl, [esi-0x1]	# 02 56 ff
        add dl, [edi-0x1]	# 02 57 ff

        add bl, [eax-1]		# 02 58 ff
        add bl, [ecx-1]		# 02 59 ff
        add bl, [edx-1]		# 02 5a ff
        add bl, [ebx-1]		# 02 5b ff
        add bl, [eax+eax*1-0x1]	# 02 5c 00 ff
        add bl, [ebp-0x1]	# 02 5d ff
        add bl, [esi-0x1]	# 02 5e ff
        add bl, [edi-0x1]	# 02 5f ff

        add ah, [eax-1]		# 02 60 ff
        add ah, [ecx-1]		# 02 61 ff
        add ah, [edx-1]		# 02 62 ff
        add ah, [ebx-1]		# 02 63 ff
        add ah, [eax+eax*1-0x1]	# 02 64 00 ff
        add ah, [ebp-0x1]	# 02 65 ff
        add ah, [esi-0x1]	# 02 66 ff
        add ah, [edi-0x1]	# 02 67 ff

        add ch, [eax-1]		# 02 68 ff
        add ch, [ecx-1]		# 02 69 ff
        add ch, [edx-1]		# 02 6a ff
        add ch, [ebx-1]		# 02 6b ff
        add ch, [eax+eax*1-0x1]	# 02 6c 00 ff
        add ch, [ebp-0x1]	# 02 6d ff
        add ch, [esi-0x1]	# 02 6e ff
        add ch, [edi-0x1]	# 02 6f ff

        add dh, [eax-1]		# 02 70 ff
        add dh, [ecx-1]		# 02 71 ff
        add dh, [edx-1]		# 02 72 ff
        add dh, [ebx-1]		# 02 73 ff
        add dh, [eax+eax*1-0x1]	# 02 74 00 ff
        add dh, [ebp-0x1]	# 02 75 ff
        add dh, [esi-0x1]	# 02 76 ff
        add dh, [edi-0x1]	# 02 77 ff

        add bh, [eax-1]		# 02 78 ff
        add bh, [ecx-1]		# 02 79 ff
        add bh, [edx-1]		# 02 7a ff
        add bh, [ebx-1]		# 02 7b ff
        add bh, [eax+eax*1-0x1]	# 02 7c 00 ff
        add bh, [ebp-0x1]	# 02 7d ff
        add bh, [esi-0x1]	# 02 7e ff
        add bh, [edi-0x1]	# 02 7f ff

        add al, [eax+0x12345678]	 # 02 80 .. .. .. ..
        add al, [ecx+0x12345678]	 # 02 81 .. .. .. ..
        add al, [edx+0x12345678]	 # 02 82 .. .. .. ..
        add al, [ebx+0x12345678]	 # 02 83 .. .. .. ..
        add al, [eax+eax*1+0x12345678]	 # 02 84 00 .. .. .. ..
        add al, [ebp+0x12345678]	 # 02 85 .. .. .. ..
        add al, [esi+0x12345678]	 # 02 86 .. .. .. ..
        add al, [edi+0x12345678]	 # 02 87 .. .. .. ..

        add cl, [eax+0x12345678]	 # 02 88 .. .. .. ..
        add cl, [ecx+0x12345678]	 # 02 89 .. .. .. ..
        add cl, [edx+0x12345678]	 # 02 8a .. .. .. ..
        add cl, [ebx+0x12345678]	 # 02 8b .. .. .. ..
        add cl, [eax+eax*1+0x12345678]	 # 02 8c 00 .. .. .. ..
        add cl, [ebp+0x12345678]	 # 02 8d .. .. .. ..
        add cl, [esi+0x12345678]	 # 02 8e .. .. .. ..
        add cl, [edi+0x12345678]	 # 02 8f .. .. .. ..

        add dl, [eax+0x12345678]	 # 02 90 .. .. .. ..
        add dl, [ecx+0x12345678]	 # 02 91 .. .. .. ..
        add dl, [edx+0x12345678]	 # 02 92 .. .. .. ..
        add dl, [ebx+0x12345678]	 # 02 93 .. .. .. ..
        add dl, [eax+eax*1+0x12345678]	 # 02 94 00 .. .. .. ..
        add dl, [ebp+0x12345678]	 # 02 95 .. .. .. ..
        add dl, [esi+0x12345678]	 # 02 96 .. .. .. ..
        add dl, [edi+0x12345678]	 # 02 97 .. .. .. ..

        add bl, [eax+0x12345678]	 # 02 98 .. .. .. ..
        add bl, [ecx+0x12345678]	 # 02 99 .. .. .. ..
        add bl, [edx+0x12345678]	 # 02 9a .. .. .. ..
        add bl, [ebx+0x12345678]	 # 02 9b .. .. .. ..
        add bl, [eax+eax*1+0x12345678]	 # 02 9c 00 .. .. .. ..
        add bl, [ebp+0x12345678]	 # 02 9d .. .. .. ..
        add bl, [esi+0x12345678]	 # 02 9e .. .. .. ..
        add bl, [edi+0x12345678]	 # 02 9f .. .. .. ..

        add ah, [eax+0x12345678]	 # 02 a0 .. .. .. ..
        add ah, [ecx+0x12345678]	 # 02 a1 .. .. .. ..
        add ah, [edx+0x12345678]	 # 02 a2 .. .. .. ..
        add ah, [ebx+0x12345678]	 # 02 a3 .. .. .. ..
        add ah, [eax+eax*1+0x12345678]	 # 02 a4 00 .. .. .. ..
        add ah, [ebp+0x12345678]	 # 02 a5 .. .. .. ..
        add ah, [esi+0x12345678]	 # 02 a6 .. .. .. ..
        add ah, [edi+0x12345678]	 # 02 a7 .. .. .. ..

        add ch, [eax+0x12345678]	 # 02 a8 .. .. .. ..
        add ch, [ecx+0x12345678]	 # 02 a9 .. .. .. ..
        add ch, [edx+0x12345678]	 # 02 aa .. .. .. ..
        add ch, [ebx+0x12345678]	 # 02 ab .. .. .. ..
        add ch, [eax+eax*1+0x12345678]	 # 02 ac 00 .. .. .. ..
        add ch, [ebp+0x12345678]	 # 02 ad .. .. .. ..
        add ch, [esi+0x12345678]	 # 02 ae .. .. .. ..
        add ch, [edi+0x12345678]	 # 02 af .. .. .. ..

        add dh, [eax+0x12345678]	 # 02 b0 .. .. .. ..
        add dh, [ecx+0x12345678]	 # 02 b1 .. .. .. ..
        add dh, [edx+0x12345678]	 # 02 b2 .. .. .. ..
        add dh, [ebx+0x12345678]	 # 02 b3 .. .. .. ..
        add dh, [eax+eax*1+0x12345678]	 # 02 b4 00 .. .. .. ..
        add dh, [ebp+0x12345678]	 # 02 b5 .. .. .. ..
        add dh, [esi+0x12345678]	 # 02 b6 .. .. .. ..
        add dh, [edi+0x12345678]	 # 02 b7 .. .. .. ..

        add bh, [eax+0x12345678]	 # 02 b8 .. .. .. ..
        add bh, [ecx+0x12345678]	 # 02 b9 .. .. .. ..
        add bh, [edx+0x12345678]	 # 02 ba .. .. .. ..
        add bh, [ebx+0x12345678]	 # 02 bb .. .. .. ..
        add bh, [eax+eax*1+0x12345678]	 # 02 bc 00 .. .. .. ..
        add bh, [ebp+0x12345678]	 # 02 bd .. .. .. ..
        add bh, [esi+0x12345678]	 # 02 be .. .. .. ..
        add bh, [edi+0x12345678]	 # 02 bf .. .. .. ..

        add al,al                                # 02 c0
        add al,cl                                # 02 c1
        add al,dl                                # 02 c2
        add al,bl                                # 02 c3
        add al,ah                                # 02 c4
        add al,ch                                # 02 c5
        add al,dh                                # 02 c6
        add al,bh                                # 02 c7

        add cl,al                                # 02 c8
        add cl,cl                                # 02 c9
        add cl,dl                                # 02 ca
        add cl,bl                                # 02 cb
        add cl,ah                                # 02 cc
        add cl,ch                                # 02 cd
        add cl,dh                                # 02 ce
        add cl,bh                                # 02 cf

        add dl,al                                # 02 d0
        add dl,cl                                # 02 d1
        add dl,dl                                # 02 d2
        add dl,bl                                # 02 d3
        add dl,ah                                # 02 d4
        add dl,ch                                # 02 d5
        add dl,dh                                # 02 d6
        add dl,bh                                # 02 d7

        add dl,al                                # 02 d8
        add dl,cl                                # 02 d9
        add dl,dl                                # 02 da
        add dl,bl                                # 02 db
        add dl,ah                                # 02 dc
        add dl,ch                                # 02 dd
        add dl,dh                                # 02 de
        add dl,bh                                # 02 df

        add ah,al                                # 02 e0
        add ah,cl                                # 02 e1
        add ah,dl                                # 02 e2
        add ah,bl                                # 02 e3
        add ah,ah                                # 02 e4
        add ah,ch                                # 02 e5
        add ah,dh                                # 02 e6
        add ah,bh                                # 02 e7

        add ch,al                                # 02 e8
        add ch,cl                                # 02 e9
        add ch,dl                                # 02 ea
        add ch,bl                                # 02 eb
        add ch,ah                                # 02 ec
        add ch,ch                                # 02 ed
        add ch,dh                                # 02 ee
        add ch,bh                                # 02 ef

        add dh,al                                # 02 f0
        add dh,cl                                # 02 f1
        add dh,dl                                # 02 f2
        add dh,bl                                # 02 f3
        add dh,ah                                # 02 f4
        add dh,ch                                # 02 f5
        add dh,dh                                # 02 f6
        add dh,bh                                # 02 f7

        add bh,al                                # 02 f8
        add bh,cl                                # 02 f9
        add bh,dl                                # 02 fa
        add bh,bl                                # 02 fb
        add bh,ah                                # 02 fc
        add bh,ch                                # 02 fd
        add bh,dh                                # 02 fe
        add bh,bh                                # 02 ff

        add eax, [eax]		         # 03 00
        add eax, [ecx]		         # 03 01
        add eax, [edx]		         # 03 02
        add eax, [ebx]		         # 03 03
        add eax, [eax+eax*1]	         # 03 04 00
        add eax, [0x12345678]	         # 03 05 78 56 34 12
        add eax, [esi]		         # 03 06
        add eax, [edi]		         # 03 07

        add ecx, [eax]		         # 03 08
        add ecx, [ecx]		         # 03 09
        add ecx, [edx]		         # 03 0a
        add ecx, [ebx]		         # 03 0b
        add ecx, [eax+eax*1]	         # 03 0c 00
        add ecx, [0x12345678]	         # 03 0d 78 56 34 12
        add ecx, [esi]		         # 03 0e
        add ecx, [edi]		         # 03 0f

        add edx, [eax]		         # 03 10
        add edx, [ecx]		         # 03 11
        add edx, [edx]		         # 03 12
        add edx, [ebx]		         # 03 13
        add edx, [eax+eax*1]	         # 03 14 00
        add edx, [0x12345678]	         # 03 15 78 56 34 12
        add edx, [esi]		         # 03 16
        add edx, [edi]		         # 03 17

        add ebx, [eax]		         # 03 18
        add ebx, [ecx]		         # 03 19
        add ebx, [edx]		         # 03 1a
        add ebx, [ebx]		         # 03 1b
        add ebx, [eax+eax*1]	         # 03 1c 00
        add ebx, [0x12345678]	         # 03 1d 78 56 34 12
        add ebx, [esi]		         # 03 1e
        add ebx, [edi]		         # 03 1f

        add esp, [eax]		         # 03 20
        add esp, [ecx]		         # 03 21
        add esp, [edx]		         # 03 22
        add esp, [ebx]		         # 03 23
        add esp, [eax+eax*1]	         # 03 24 00
        add esp, [0x12345678]	         # 03 25 78 56 34 12
        add esp, [esi]		         # 03 26
        add esp, [edi]		         # 03 27

        add ebp, [eax]		         # 03 28
        add ebp, [ecx]		         # 03 29
        add ebp, [edx]		         # 03 2a
        add ebp, [ebx]		         # 03 2b
        add ebp, [eax+eax*1]	         # 03 2c 00
        add ebp, [0x12345678]	         # 03 2d 78 56 34 12
        add ebp, [esi]		         # 03 2e
        add ebp, [edi]		         # 03 2f

        add esi, [eax]		         # 03 30
        add esi, [ecx]		         # 03 31
        add esi, [edx]		         # 03 32
        add esi, [ebx]		         # 03 33
        add esi, [eax+eax*1]	         # 03 34 00
        add esi, [0x12345678]	         # 03 35 78 56 34 12
        add esi, [esi]		         # 03 36
        add esi, [edi]		         # 03 37

        add edi, [eax]		         # 03 38
        add edi, [ecx]		         # 03 39
        add edi, [edx]		         # 03 3a
        add edi, [ebx]		         # 03 3b
        add edi, [eax+eax*1]	         # 03 3c 00
        add edi, [0x12345678]	         # 03 3d 78 56 34 12
        add edi, [esi]		         # 03 3e
        add edi, [edi]		         # 03 3f

        add eax, [eax-0x1]              # 03 40 ff
        add eax, [ecx-0x1]              # 03 41 ff
        add eax, [edx-0x1]              # 03 42 ff
        add eax, [ebx-0x1]              # 03 43 ff
        add eax, [eax+eax*1-0x1]        # 03 44 00 ff
        add eax, [ebp-0x1]              # 03 45 ff
        add eax, [esi-0x1]              # 03 46 ff
        add eax, [edi-0x1]              # 03 47 ff

        add ecx, [eax-0x1]              # 03 48 ff
        add ecx, [ecx-0x1]              # 03 49 ff
        add ecx, [edx-0x1]              # 03 4a ff
        add ecx, [ebx-0x1]              # 03 4b ff
        add ecx, [eax+eax*1-0x1]        # 03 4c 00 ff
        add ecx, [ebp-0x1]              # 03 4d ff
        add ecx, [esi-0x1]              # 03 4e ff
        add ecx, [edi-0x1]              # 03 4f ff

        add edx, [eax-0x1]              # 03 50 ff
        add edx, [ecx-0x1]              # 03 51 ff
        add edx, [edx-0x1]              # 03 52 ff
        add edx, [ebx-0x1]              # 03 53 ff
        add edx, [eax+eax*1-0x1]        # 03 54 00 ff
        add edx, [ebp-0x1]              # 03 55 ff
        add edx, [esi-0x1]              # 03 56 ff
        add edx, [edi-0x1]              # 03 57 ff

        add ebx, [eax-0x1]              # 03 58 ff
        add ebx, [ecx-0x1]              # 03 59 ff
        add ebx, [edx-0x1]              # 03 5a ff
        add ebx, [ebx-0x1]              # 03 5b ff
        add ebx, [eax+eax*1-0x1]        # 03 5c 00 ff
        add ebx, [ebp-0x1]              # 03 5d ff
        add ebx, [esi-0x1]              # 03 5e ff
        add ebx, [edi-0x1]              # 03 5f ff

        add esp, [eax-0x1]              # 03 60 ff
        add esp, [ecx-0x1]              # 03 61 ff
        add esp, [edx-0x1]              # 03 62 ff
        add esp, [ebx-0x1]              # 03 63 ff
        add esp, [eax+eax*1-0x1]        # 03 64 00 ff
        add esp, [ebp-0x1]              # 03 65 ff
        add esp, [esi-0x1]              # 03 66 ff
        add esp, [edi-0x1]              # 03 67 ff

        add ebp, [eax-0x1]              # 03 68 ff
        add ebp, [ecx-0x1]              # 03 69 ff
        add ebp, [edx-0x1]              # 03 6a ff
        add ebp, [ebx-0x1]              # 03 6b ff
        add ebp, [eax+eax*1-0x1]        # 03 6c 00 ff
        add ebp, [ebp-0x1]              # 03 6d ff
        add ebp, [esi-0x1]              # 03 6e ff
        add ebp, [edi-0x1]              # 03 6f ff

        add esp, [eax-0x1]              # 03 60 ff
        add esp, [ecx-0x1]              # 03 61 ff
        add esp, [edx-0x1]              # 03 62 ff
        add esp, [ebx-0x1]              # 03 63 ff
        add esp, [eax+eax*1-0x1]        # 03 64 00 ff
        add esp, [ebp-0x1]              # 03 65 ff
        add esp, [esi-0x1]              # 03 66 ff
        add esp, [edi-0x1]              # 03 67 ff

        add ebp, [eax-0x1]              # 03 68 ff
        add ebp, [ecx-0x1]              # 03 69 ff
        add ebp, [edx-0x1]              # 03 6a ff
        add ebp, [ebx-0x1]              # 03 6b ff
        add ebp, [eax+eax*1-0x1]        # 03 6c 00 ff
        add ebp, [ebp-0x1]              # 03 6d ff
        add ebp, [esi-0x1]              # 03 6e ff
        add ebp, [edi-0x1]              # 03 6f ff

        add esi, [eax-0x1]              # 03 70 ff
        add esi, [ecx-0x1]              # 03 71 ff
        add esi, [edx-0x1]              # 03 72 ff
        add esi, [ebx-0x1]              # 03 73 ff
        add esi, [eax+eax*1-0x1]        # 03 74 00 ff
        add esi, [ebp-0x1]              # 03 75 ff
        add esi, [esi-0x1]              # 03 76 ff
        add esi, [edi-0x1]              # 03 77 ff

        add edi, [eax-0x1]              # 03 78 ff
        add edi, [ecx-0x1]              # 03 79 ff
        add edi, [edx-0x1]              # 03 7a ff
        add edi, [ebx-0x1]              # 03 7b ff
        add edi, [eax+eax*1-0x1]        # 03 7c 00 ff
        add edi, [ebp-0x1]              # 03 7d ff
        add edi, [esi-0x1]              # 03 7e ff
        add edi, [edi-0x1]              # 03 7f ff

        add eax, [eax+0x12345678]       # 03 80 78 56 34 12
        add eax, [ecx+0x12345678]       # 03 81 78 56 34 12
        add eax, [edx+0x12345678]       # 03 82 78 56 34 12
        add eax, [ebx+0x12345678]       # 03 83 78 56 34 12
        add eax, [eax+eax*1+0x12345678] # 03 84 00 78 56 34 12
        add eax, [ebp+0x12345678]       # 03 85 78 56 34 12
        add eax, [esi+0x12345678]       # 03 86 78 56 34 12
        add eax, [edi+0x12345678]       # 03 87 78 56 34 12

        add ecx, [eax+0x12345678]       # 03 88 78 56 34 12
        add ecx, [ecx+0x12345678]       # 03 89 78 56 34 12
        add ecx, [edx+0x12345678]       # 03 8a 78 56 34 12
        add ecx, [ebx+0x12345678]       # 03 8b 78 56 34 12
        add ecx, [eax+eax*1+0x12345678] # 03 8c 00 78 56 34 12
        add ecx, [ebp+0x12345678]       # 03 8d 78 56 34 12
        add ecx, [esi+0x12345678]       # 03 8e 78 56 34 12
        add ecx, [edi+0x12345678]       # 03 8f 78 56 34 12

        add edx, [eax+0x12345678]       # 03 90 78 56 34 12
        add edx, [ecx+0x12345678]       # 03 91 78 56 34 12
        add edx, [edx+0x12345678]       # 03 92 78 56 34 12
        add edx, [ebx+0x12345678]       # 03 93 78 56 34 12
        add edx, [eax+eax*1+0x12345678] # 03 94 00 78 56 34 12
        add edx, [ebp+0x12345678]       # 03 95 78 56 34 12
        add edx, [esi+0x12345678]       # 03 96 78 56 34 12
        add edx, [edi+0x12345678]       # 03 97 78 56 34 12

        add ebx, [eax+0x12345678]       # 03 98 78 56 34 12
        add ebx, [ecx+0x12345678]       # 03 99 78 56 34 12
        add ebx, [edx+0x12345678]       # 03 9a 78 56 34 12
        add ebx, [ebx+0x12345678]       # 03 9b 78 56 34 12
        add ebx, [eax+eax*1+0x12345678] # 03 9c 00 78 56 34 12
        add ebx, [ebp+0x12345678]       # 03 9d 78 56 34 12
        add ebx, [esi+0x12345678]       # 03 9e 78 56 34 12
        add ebx, [edi+0x12345678]       # 03 9f 78 56 34 12

        add esp, [eax+0x12345678]       # 03 a0 78 56 34 12
        add esp, [ecx+0x12345678]       # 03 a1 78 56 34 12
        add esp, [edx+0x12345678]       # 03 a2 78 56 34 12
        add esp, [ebx+0x12345678]       # 03 a3 78 56 34 12
        add esp, [eax+eax*1+0x12345678] # 03 a4 00 78 56 34 12
        add esp, [ebp+0x12345678]       # 03 a5 78 56 34 12
        add esp, [esi+0x12345678]       # 03 a6 78 56 34 12
        add esp, [edi+0x12345678]       # 03 a7 78 56 34 12

        add ebp, [eax+0x12345678]       # 03 a8 78 56 34 12
        add ebp, [ecx+0x12345678]       # 03 a9 78 56 34 12
        add ebp, [edx+0x12345678]       # 03 aa 78 56 34 12
        add ebp, [ebx+0x12345678]       # 03 ab 78 56 34 12
        add ebp, [eax+eax*1+0x12345678] # 03 ac 00 78 56 34 12
        add ebp, [ebp+0x12345678]       # 03 ad 78 56 34 12
        add ebp, [esi+0x12345678]       # 03 ae 78 56 34 12
        add ebp, [edi+0x12345678]       # 03 af 78 56 34 12

        add esi, [eax+0x12345678]       # 03 b0 78 56 34 12
        add esi, [ecx+0x12345678]       # 03 b1 78 56 34 12
        add esi, [edx+0x12345678]       # 03 b2 78 56 34 12
        add esi, [ebx+0x12345678]       # 03 b3 78 56 34 12
        add esi, [eax+eax*1+0x12345678] # 03 b4 00 78 56 34 12
        add esi, [ebp+0x12345678]       # 03 b5 78 56 34 12
        add esi, [esi+0x12345678]       # 03 b6 78 56 34 12
        add esi, [edi+0x12345678]       # 03 b7 78 56 34 12

        add edi, [eax+0x12345678]       # 03 b8 78 56 34 12
        add edi, [ecx+0x12345678]       # 03 b9 78 56 34 12
        add edi, [edx+0x12345678]       # 03 ba 78 56 34 12
        add edi, [ebx+0x12345678]       # 03 bb 78 56 34 12
        add edi, [eax+eax*1+0x12345678] # 03 bc 00 78 56 34 12
        add edi, [ebp+0x12345678]       # 03 bd 78 56 34 12
        add edi, [esi+0x12345678]       # 03 be 78 56 34 12
        add edi, [edi+0x12345678]       # 03 bf 78 56 34 12

        add eax,eax                              # 03 c0
        add eax,ecx                              # 03 c1
        add eax,edx                              # 03 c2
        add eax,ebx                              # 03 c3
        add eax,esp                              # 03 c4
        add eax,ebp                              # 03 c5
        add eax,esi                              # 03 c6
        add eax,edi                              # 03 c7

        add ecx,eax                              # 03 c8
        add ecx,ecx                              # 03 c9
        add ecx,edx                              # 03 ca
        add ecx,ebx                              # 03 cb
        add ecx,esp                              # 03 cc
        add ecx,ebp                              # 03 cd
        add ecx,esi                              # 03 ce
        add ecx,edi                              # 03 cf

        add edx,eax                              # 03 d0
        add edx,ecx                              # 03 d1
        add edx,edx                              # 03 d2
        add edx,ebx                              # 03 d3
        add edx,esp                              # 03 d4
        add edx,ebp                              # 03 d5
        add edx,esi                              # 03 d6
        add edx,edi                              # 03 d7

        add ebx,eax                              # 03 d8
        add ebx,ecx                              # 03 d9
        add ebx,edx                              # 03 da
        add ebx,ebx                              # 03 db
        add ebx,esp                              # 03 dc
        add ebx,ebp                              # 03 dd
        add ebx,esi                              # 03 de
        add ebx,edi                              # 03 df

        add esp,eax                              # 03 e0
        add esp,ecx                              # 03 e1
        add esp,edx                              # 03 e2
        add esp,ebx                              # 03 e3
        add esp,esp                              # 03 e4
        add esp,ebp                              # 03 e5
        add esp,esi                              # 03 e6
        add esp,edi                              # 03 e7

        add ebp,eax                              # 03 e8
        add ebp,ecx                              # 03 e9
        add ebp,edx                              # 03 ea
        add ebp,ebx                              # 03 eb
        add ebp,esp                              # 03 ec
        add ebp,ebp                              # 03 ed
        add ebp,esi                              # 03 ee
        add ebp,edi                              # 03 ef

        add esi,eax                              # 03 f0
        add esi,ecx                              # 03 f1
        add esi,edx                              # 03 f2
        add esi,ebx                              # 03 f3
        add esi,esp                              # 03 f4
        add esi,ebp                              # 03 f5
        add esi,esi                              # 03 f6
        add esi,edi                              # 03 f7

        add edi,eax                              # 03 f8
        add edi,ecx                              # 03 f9
        add edi,edx                              # 03 fa
        add edi,ebx                              # 03 fb
        add edi,esp                              # 03 fc
        add edi,ebp                              # 03 fd
        add edi,esi                              # 03 fe
        add edi,edi                              # 03 ff

        add al,0x12				 # 04 12

        add eax,0x12345678			 # 05 78 56 34 12


        add  [eax],al           # 00 00
        add  [eax],ah           # 00 20
        add  [eax],eax          # 01 00
        add  [eax],ax           # 66 01 00

        add byte ptr [bx+si],al # 67 00 00
        add dword ptr [bx+si],eax # 67 01 00

        .byte 0x67, 0x00, 0x00

        add edi,edi                              # 01 ff
        add di,di                                # 66 01 ff
        .byte 0x66, 0x03, 0xff

        add al,0x12				 # 04 12

w:	.space 2

        .set $mw, w
        add ax,$mw				 # 66 05 34 12
        .set $mw, 0x2345
        add ax,$mw				 # 66 05 34 12

        $tw = 0x1234
        add ax,$tw				 # 66 05 34 12
        add eax,0x12345678			 # 05 78 56 34 12

        xacquire lock add word ptr ss:[si+0x1234],0x5678
        .byte 0xF2, 0xF0, 0x36, 0x66, 0x67, 0x81, 0x84, 0x34, 0x12, 0x78, 0x56

        .byte 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90


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
