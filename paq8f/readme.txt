paq8f is an open source (GPL) file compressor and archiver.
All code is by Matt Mahoney unless specified otherwise.
Last update, Feb. 2. 2007.

Contents of paq8f.zip:

readme.txt - this file
paq8f.exe - Win32 executable for Pentium MMX and higher
paq8f32 - 32 bit Linux executable (compiled by Giorgio Tani)
paq8f64 - 64 bit Linux executable
paq8f.cpp - C++ source code for all versions
paq7asm.asm - NASM/YASM assembler code for Pentium MMX or higher (tested in Windows)
paq7asmsse.asm - NASM/YASM for Pentium 4 (SSE2) or higher in 32 bit mode (tested in Windows) by wowtiger
paq7asm-x86_64 - YASM for x86-64 bit processors (tested in 64 bit Linux) partially by Matthew Fite
paq7asm-x86_64.o - above, assembled for 64 bit Linux.

paq8f can be compiled for other processors without the assembler
code using the -DNOASM option (but it will run slower).
The assembler code is the same for all paq7/8 versions.
