paq8hp12 is an open source (GPL) file compressor and archiver for English texts.
The code is written mainly by Matt Mahoney, Alexander Ratushnyak and Przemyslaw Skibinski
Last update: 8 Jan 2009

Contents of paq8hp12any_src.zip:

to_train_models.dic - data used to initialize models
temp_HKCC_dict1.dic - dictionary used to transform the data you (de)compress
	Both files must be in the current folder, together with the executable.
paq7asm.asm    - NASM/YASM assembler code for Pentium MMX or higher (tested in Windows)
paq7asmsse.asm - NASM/YASM for Pentium 4 (SSE2) or higher in 32-bit mode (tested in Windows)
paq7asmsse.obj - above, assembled for Windows
paq7asm-x86_64.asm - YASM for x86-64 bit processors (tested in 64-bit Linux)
paq7asm-x86_64.o   - above, assembled for 64-bit Linux
paq8hp12.cpp   - C++ source code
textfilter.hpp - C++ code for the WRT transform
paq8hp12.exe - Win32 executable for Pentium 4 and higher, compiled with MinGW 3.4.2:
		g++.exe -O2 -s -march=pentiumpro -fomit-frame-pointer -DWIN32 -DNO_UNWRT_CHECK paq8hp12.cpp PAQ7asmsse.OBJ
paq8hp12_l64 - 64-bit Linux executable, compiled with GCC 4.1.2 20071124:
		g++ -O2 -s -fmove-loop-invariants -DNO_UNWRT_CHECK paq8hp12.cpp paq7asm-x86_64.o
readme.txt - this file

paq8hp12 can be compiled for other processors without the assembler code
using the -DNOASM option (but it will run slower).
The assembler code is the same for all paq7/8 versions.
