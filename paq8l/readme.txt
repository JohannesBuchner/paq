paq8l is an open source (GPL) file compressor and archiver.
Last update Mar. 18, 2007 by Matt Mahoney.

Contents of paq8l.zip:

readme.txt - this file
paq8l.exe - Win32 (MinGW g++) executable for Pentium MMX and higher
paq-8l_intel.exe - Faster Win32 executable (compiled by Johan de Bock with Intel C++ from http://uclc.info )
paq8l - Linux executable (by Giorgio Tani, Mar. 18, 2007)

paq8l.cpp - C++ source code for all versions (Mar. 8, 2007)
paq7asm.asm - NASM/YASM assembler code for Pentium MMX or higher
paq7asmsse.asm - NASM/YASM for Pentium 4 (SSE2) or higher in 32 bit mode
paq7asm-x86_64.asm - YASM for x86-64 bit processors (tested in 64 bit Linux)

paq8l can be compiled for other processors without the assembler
code using the -DNOASM option (but it will run slower).
The assembler code is the same for all paq7/8 versions.

paq8l was written by Matt Mahoney (as paq8f) with improvements by
Bill Pettis (based on improvements by Alexander Ratushnyak and
Przemyslaw Skibinski in the paq8hp* series) and Serge Osnach (additional
models), and Andrew Paterson (Borland port).  The assembler code was ported 
to 64 bit by Matthew Fite and 32 bit SSE2 by wowtiger.

Other contributors to the PAQ project: Berto Destasio (tuning earlier
models for better compression), Johan de Bock (benchmarking, compiling
fast exectuables), David A. Scott (arithmetic coder improvements),
Fabio Buffoni (speed optimizations), Jason Schmidt (compression
improvements), Rudi Cilibrasi (text modeling), and Pavel L. Holoborodko
(PGM image modeling), and Jari Aalto (licensing/distribution).

This work would not be possible without the benchmarking efforts of
Marcus Hutter (Hutter prize), Werner Bergmans (maximumcompression.com)
Johan de Bock (UCLC), Berto Destasio (Emilcont benchmark), Stephan Busch
(Squeeze Chart), Leonid A. Broukhis (Calgary Corpus Challenge),
and Black Fox.

A similar (but rewritten) context mixing algorithm is used in
WinRK 3.0.3 (pwcm mode) by Malcolm Taylor.  Modified versions of 
PAQ (faster but less compression) are used in UDA and WinUDA by dwing,
and in xml-wrt by Przemyslaw Skibinski.

