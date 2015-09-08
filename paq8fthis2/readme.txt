paq8fthis2 (C) 2005, 2007, Matt Mahoney, Jan Ondrus
This is free software under GPL, http://www.gnu.org/licenses/gpl.txt
Released Aug. 12, 2007.

paq8fthis2 is the paq8f archiver modified by Jan Ondrus to improve
compression of JPEG files.  It is faster and compresses JPEG better
than paq8fthis.  Except for JPEG data, compression is the same as paq8f.

paq8fthis2.exe was compiled with MINGW g++ 3.4.5 as follows:

nasm paq7asm.asm -f win32 --prefix _
g++ -Wall paq8fthis2.cpp -O2 -Os -march=pentiumpro -fomit-frame-pointer -s -o paq8fthis2.exe paq7asm.obj -DWINDOWS

paq7asm.obj was assembled with nasm and will link if compiled with MinGW g++
for 32 bit Windows.  To link with other compilers, see the source code instructions.
