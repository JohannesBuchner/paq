; YASM x86-64 assembly language code for PAQ7/8 ver. 2, Jan 18, 2007
;
; (C) 2005-2007, Matt Mahoney, Matthew Fite.
; This is free software under GPL, http://www.gnu.org/licenses/gpl.txt
;
; This code was tested on an Athlon-64 under Ubuntu Linux 2.6.15.27.amd64-generic
; with paq8f and paq8jd.  It should work with any PAQ version since paq7,
; because all versions use the same paq7asm.asm code for 32 bit Windows/Linux
; versions.  To compile e.g. paq8jd in Linux:
;
;   yasm paq7asm-x86_64.asm -f elf -m amd64
;   g++ -O3 -s -fomit-frame-pointer -DUNIX paq8jd.cpp paq7asm-x86_64.o -o paq8jd
;
; This code has not been tested in Windows.  (You would need XP Professional
; 64 bit edition and a 64 bit compiler).

section .text

BITS 64

; Vector product a*b of n signed words, returning signed dword scaled
; down by 8 bits. n is rounded up to a multiple of 8.

    global dot_product ; (short* a, short* b, int n)
    align 16
dot_product:
    mov rcx, rdx        ; n
    mov rax, rdi        ; a
    mov rdx, rsi        ; b
    add rcx, 7          ; n rounding up
    and rcx, -8
    jz .done
    sub rax, 16
    sub rdx, 16
    pxor xmm0, xmm0     ; sum = 0
.loop:                  ; each loop sums 4 products
    movdqa xmm1, [rax+rcx*2] ; put parital sums of vector product in xmm1
    pmaddwd xmm1, [rdx+rcx*2]
    psrad xmm1, 8
    paddd xmm0, xmm1
    sub rcx, 8
    ja .loop
    movdqa xmm1, xmm0      ; add 4 parts of xmm0 and return in eax
    psrldq xmm1, 8
    paddd xmm0, xmm1
    movdqa xmm1, xmm0
    psrldq xmm1, 4
    paddd xmm0, xmm1
    movd rax, xmm0
.done
    ret

; Train n neural network weights w[n] on inputs t[n] and err.
; w[i] += (t[i]*err*2 >> 16)+1 >> 1 bounded to +- 32K.
; n is rounded up to a multiple of 8.

;1st arg rdi -> *t
;2nd arg rsi -> *w
;3rd arg rdx ->  n
;4th arg rcx ->  err (signed 16 bits)

    global train ; (short* t, short* w, int n, int err)
    BITS 64
    align 16
train:
    mov rax, rcx          ; err
    and rax, 0xffff       ; put 8 copies of err in xmm0
    movd xmm0, rax
    movd xmm1, rax
    pslldq xmm1, 2
    por xmm0, xmm1
    movdqa xmm1, xmm0
    pslldq xmm1, 4
    por xmm0, xmm1
    movdqa xmm1, xmm0
    pslldq xmm1, 8
    por xmm0, xmm1;
    pcmpeqb xmm1, xmm1    ; 8 copies of 1 in xmm1
    psrlw xmm1, 15
    mov rcx, rdx          ; n
    mov rax, rdi          ; t
    mov rdx, rsi          ; w
    add rcx, 7            ; n/8 rounding up
    and rcx, -8
    sub rax, 16
    sub rdx, 16
    jz .done
    align 16
.loop:                     ; each iteration adjusts 8 weights
    movdqa xmm2, [rdx+rcx*2] ; w[i]
    movdqa xmm3, [rax+rcx*2] ; t[i]
    paddsw xmm3, xmm3      ; t[i]*2
    pmulhw xmm3, xmm0      ; t[i]*err*2 >> 16
    paddsw xmm3, xmm1      ; (t[i]*err*2 >> 16)+1
    psraw xmm3, 1          ; (t[i]*err*2 >> 16)+1 >> 1
    paddsw xmm2, xmm3      ; w[i] + xmm3
    movdqa [rdx+rcx*2], xmm2
    sub rcx, 8
    ja .loop
.done:
    ret

