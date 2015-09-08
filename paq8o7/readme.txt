paq8o7 archiver, released by KZ, Oct. 15, 2007.

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
  paq8o7.exe - Win32 executable
  paq8o7.cpp - C++ source code.
  paq7asm.asm - NASM source code for 32 bit x86 (Pentium-MMX or higher).
  paq7asm.obj - assembled paq7asm.asm for linking with VC.
  paq7asmsse.asm - NASM/YASM for Pentium 4 (SSE2) or higher in 32 bit mode.
  paq7asm-x86_64.asm - YASM source code for x86-64.

paq8o7 by KZ 
Detection of BMP 4,8,24 bit and PGM 8 bit images before compress
On non BMP,PGM,JPEG data mem is lower
Fixed bug in BMP 8-bit detection in other files like .exe.
Updates JPEG model by Jan Ondrus
PGM detection bug fix

See paq8o7.cpp source comments for usage and compiling instructions.
