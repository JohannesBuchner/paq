paq8fthis3 (C) 2005, 2007, Matt Mahoney, Jan Ondrus
This is free software under GPL, http://www.gnu.org/licenses/gpl.txt
Released Sept. 8, 2007.

paq8fthis3 is the paq8f archiver modified by Jan Ondrus to improve
compression of JPEG files.  It compresses JPEG better than paq8fthis2
at about the same speed.  Except for JPEG data, compression is the same as paq8f.

paq8fthis3.exe was compiled with MINGW g++ 3.4.5 as follows:

nasm paq7asm.asm -f win32 --prefix _
g++ -Wall paq8fthis3.cpp -O2 -Os -march=pentiumpro -fomit-frame-pointer -s -o paq8fthis3.exe paq7asm.obj -DWINDOWS

paq7asm.obj was assembled with nasm and will link if compiled with MinGW g++
for 32 bit Windows.  To link with other compilers, see the source code instructions.

To compress: paq8fthis3 -N archive files...

  where -N is -0 (fastest, least memory) to -8 (smallest).  Output is archive.paq8f

To decompress: paq8fthis3 -d archive.paq8f
or:            paq8fthis3 -d archive.paq8f output_directory

  Output directory is created if needed.  Default is current directory.
  Decompression speed and memory usage is the same as compression.

