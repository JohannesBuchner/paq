; NASM assembly language code for PAQ7.
; (C) 2005, Matt Mahoney.
; train - written by wowtiger, Jan. 30, 2007
; train and dot_product - optimized by Dark Shikari, Jan. 8, 2009
;
; This is free software under GPL, http://www.gnu.org/licenses/gpl.txt
;
; This code is a replacement for paq7asm.asm for newer processors
; supporting SSE2 instructions.  It is about 1% faster than the
; equivalent MMX code.  It can be linked with any version of paq7*
; or paq8*.  Assemble as below, then link following the instructions
; in the C++ source code, replacing paq7asm.obj with paq7asmsse.obj.
; No C++ code changes are needed.
;
;   MINGW g++:     nasm paq7asmsse.asm -f win32 --prefix _
;   DJGPP g++:     nasm paq7asmsse.asm -f coff  --prefix _
;   Borland, Mars: nasm paq7asmsse.asm -f obj   --prefix _
;   Linux:         nasm paq7asmsse.asm -f elf
;

section .text use32 class=CODE

; Vector product a*b of n signed words, returning signed dword scaled
; down by 8 bits. n is rounded up to a multiple of 8.

global dot_product              ; (short* a, short* b, int n)
align 16
dot_product:
  mov      eax, [esp+4]         ; a
  mov      edx, [esp+8]         ; b
  mov      ecx, [esp+12]        ; n
  add      ecx, 7               ; n rounding up
  and      ecx, -8
  jz .done
  sub      eax, 16
  sub      edx, 16
  pxor    xmm0, xmm0            ; sum = 0
.loop:                          ; each loop sums 4 products
  movdqa  xmm1, [eax+ecx*2]     ; put partial sums of vector product in xmm0
  pmaddwd xmm1, [edx+ecx*2]
  psrad   xmm1, 8
  paddd   xmm0, xmm1
  sub ecx, 8
  ja .loop
  movhlps xmm1, xmm0            ; add 4 parts of xmm0 and return in eax
  paddd   xmm0, xmm1
  pshuflw xmm1, xmm0, 0xE
  paddd   xmm0, xmm1
  movd     eax, xmm0
.done:
  ret

; Train n neural network weights w[n] on inputs t[n] and err.
; w[i] += t[i]*err*2+1 >> 17 bounded to +- 32K.
; n is rounded up to a multiple of 16.

; Train for SSE2
; Use this code to get some performance...

global train ; (short* t, short* w, int n, int err)
align 16
train:
  mov         eax, [esp+4]          ; t
  mov         edx, [esp+8]          ; w
  mov         ecx, [esp+12]         ; n
  add         ecx, 15               ; n/16 rounding up
  and         ecx, -16
  jz .done
  sub         eax, 32
  sub         edx, 32
  movd       xmm0, [esp+16]
  pcmpeqb    xmm6, xmm6
  pshuflw    xmm0, xmm0, 0
  psrlw      xmm6, 15               ; pw_1
  punpcklqdq xmm0, xmm0
.loop:                              ; each iteration adjusts 16 weights
  movdqa     xmm3, [eax+ecx*2 +0]   ; t[i]
  movdqa     xmm5, [eax+ecx*2+16]
  paddsw     xmm3, xmm3             ; t[i]*2
  paddsw     xmm5, xmm5
  pmulhw     xmm3, xmm0             ; t[i]*err*2 >> 16
  pmulhw     xmm5, xmm0
  paddsw     xmm3, xmm6             ; (t[i]*err*2 >> 16)+1
  paddsw     xmm5, xmm6
  psraw      xmm3, 1                ; (t[i]*err*2 >> 16)+1 >> 1
  psraw      xmm5, 1
  paddsw     xmm3, [edx+ecx*2+ 0]   ; w[i] + xmm3
  paddsw     xmm5, [edx+ecx*2+16]
  movdqa [edx+ecx*2+ 0], xmm3
  movdqa [edx+ecx*2+16], xmm5
  sub     ecx, 16
  ja .loop
.done:
  ret
