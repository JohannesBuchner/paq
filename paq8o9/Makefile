# Written by Arda Saltoglu
# Licensed cc-by-sa (version 3.0)
# For license details see:
# http://creativecommons.org/licenses/by-sa/3.0/

obj-of-arch = $(shell if grep -q " sse " /proc/cpuinfo ; then echo paq7asmsse.o ; else echo paq7asm.o ; fi)

linee = "---------------------------------------------------------------"
rootrequired = "root privileges required to install to /usr/local/bin"

.PHONY : clean install

paq8o9 : $(obj-of-arch) paq8o9.cpp
	g++ paq8o9.cpp -DUNIX -DDEFAULT_OPTION=5 -O2 -Os -s -march=pentiumpro -fomit-frame-pointer -o paq8o9 $(obj-of-arch)

paq7asm.o : paq7asm.asm
	nasm -f elf paq7asm.asm

paq7asmsse.o : paq7asmsse.asm
	nasm -f elf paq7asmsse.asm

clean :
	rm -f *.o paq8o9

install : paq8o9
	@if [ `whoami` != "root" ] ; then echo $(linee) ; echo Warning: $(rootrequired) ; echo $(linee) ; echo ; fi
	install paq8o9 /usr/local/bin
