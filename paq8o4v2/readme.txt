paq8o4 ver. 2 archiver, released by Matt Mahoney, Sept. 17, 2007.

Copyright (C) 2007 Matt Mahoney, Serge Osnach, Alexander Ratushnyak,
Bill Pettis, Przemyslaw Skibinski, Matthew Fite, wowtiger, Andrew Paterson,
Jan Ondrus, Andreas Morphis, Pavel L. Holoborodko, KZ.

    LICENSE

    This program is free software; you can redistribute it and/or
    modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation; either version 2 of
    the License, or (at your option) any later version.

    This program is distributed in the hope that it will be useful, but
    WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    General Public License for more details at
    Visit <http://www.gnu.org/copyleft/gpl.html>.

See http://cs.fit.edu/~mmahoney/compression/ for the latest PAQ version.

Contents:

  readme.txt - This file.
  paq8o4.exe - Win32 executable.
  paq8o4.cpp - C++ source code.
  paq7asm.asm - NASM source code for 32 bit x86 (Pentium-MMX or higher).
  paq7asm.obj - assembled paq7asm.asm for linking with g++
  paq7asmsse.asm - NASM/YASM for Pentium 4 (SSE2) or higher in 32 bit mode.
  paq7asmsse.obj - assembled for linking with g++
  paq7asm-x86_64.asm - YASM source code for x86-64.

paq8o4 ver. 2 is archive-compatible with ver. 1 from
http://code.google.com/p/paq8/source released by KZ on Sept. 15, 2007.
It differs from ver. 1 in that it was compiled with g++ -DWINDOWS instead
of the Intel compiler, so that wildcards, directory traversal, and directory
creation work.  However, it is 8% slower.  The source code was modified to
work with C++ compilers that use either standard "for" loop scoping rules
or pre-1999 rules.

See paq8o4.cpp source comments for usage and compiling instructions.
