paq8n archiver, released Aug. 18, 2007.

Copyright (C) 2007 Matt Mahoney, Serge Osnach, Alexander Ratushnyak,
Bill Pettis, Przemyslaw Skibinski, Matthew Fite, wowtiger, Andrew Paterson,
Jan Ondrus.

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
  paq8n.exe - Win32 executable.
  paq8n.cpp - C++ source code
  paq7asm.asm - NASM source code for 32 bit x86 (Pentium-MMX or higher).
  paq7asm.obj - Assembled with NASM for linking using MinGW g++.
  paq7asm-x86_64.asm - YASM source code for x86-64.

See paq8n.cpp source comments for usage and compiling instructions.
paq8n.exe was compiled as follows with nasm 0.98.38, MinGW g++ 3.4.5
and upx 3.00w:

  nasm paq7asm.asm -f win32 --prefix _
  g++ -Wall -O2 -Os -march=pentiumpro -fomit-frame-pointer -s -DWINDOWS paq8n.cpp paq7asm.obj -o paq8n.exe
  upx paq8n.exe
