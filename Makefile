
CC := g++ -DUNIX -O2 -Os -s -m32 -fomit-frame-pointer
#CC := g++ -DUNIX -O3 -s 
TARGETS := paq8a.exe paq8f.exe paq8fthis2.exe paq8fthis3.exe paq8fthis4.exe paq8g.exe paq8hp12any.exe paq8jd.exe paq8k.exe paq8k2.exe paq8k3.exe paq8kx_v1.exe paq8kx_v4.exe paq8kx_v7.exe paq8l.exe paq8m.exe paq8n.exe paq8o.exe paq8o10t.exe paq8o2.exe paq8o3.exe paq8o4v2.exe paq8o5.exe paq8o6.exe paq8o7.exe paq8o8.exe paq8o9.exe paq8p.exe paq8px_v1.exe paq8px_v44.exe paq8px_v67.exe paq8px_v68e.exe paq8px_v68p3.exe paq8px_v9.exe

all: ${TARGETS}
clean:
	rm -f ${TARGETS} */*.o

%.o: %.asm
	nasm -f elf $?

paq8a.exe: paq8a/paq8a.cpp paq8o10t/paq7asm.o
	${CC} -o $@ $?

#paq8b.exe: paq8b/src/Paq8b.cpp paq8b/src/TextFilter.cpp ./paq8b/src/Paq8asm.o
#	${CC} -o $@ $?

#paq8c.exe: paq8c/paq8c.cpp paq8c/paq7asm.o
#	${CC} -o $@ $?

#paq8d.exe: paq8d/paq8d.cpp paq8d/paq7asm.o
#	${CC} -o $@ $?

#paq8e.exe: paq8e/paq8e.cpp paq8e/paq7asm.o
#	${CC} -o $@ $?

paq8f.exe: paq8f/paq8f.cpp paq8f/paq7asm.o
	${CC} -o $@ $?

paq8fthis2.exe: paq8fthis2/paq8fthis2.cpp paq8fthis2/paq7asm.o
	${CC} -o $@ $?

paq8fthis3.exe: paq8fthis3/paq8fthis3.cpp paq8fthis3/paq7asm.o
	${CC} -o $@ $?

paq8fthis4.exe: paq8fthis4/paq8fthis4.cpp paq8fthis4/paq7asm.o
	${CC} -o $@ $?

paq8g.exe: paq8g/src/paq8g.cpp ./paq8g/src/paq8asm.o
	${CC} -o $@ $?

paq8hp12any.exe: paq8hp12any/paq8hp12.cpp paq8hp12any/paq7asm.o
	${CC} -o $@ $?

paq8i.exe: paq8i/paq8i.cpp paq8i/paq7asm.o
	${CC} -o $@ $?

paq8jd.exe: paq8jd/paq8jd.cpp paq8jd/paq7asm.o
	${CC} -o $@ $?

paq8k.exe: paq8k/paq8k.cpp paq8k/paq7asm.o
	${CC} -o $@ $?

paq8k2.exe: paq8k2/paq8k2.cpp paq8k2/paq7asm.o
	${CC} -o $@ $?

paq8k3.exe: paq8k3/paq8k3.cpp paq8k3/paq7asm.o
	${CC} -o $@ $?

paq8kx_v1.exe: paq8kx_v1/paq8kx_v1.cpp paq8kx_v1/paq7asm.o
	${CC} -o $@ $?

paq8kx_v4.exe: paq8kx_v4/paq8kx_v4.cpp paq8kx_v4/paq7asm.o
	${CC} -o $@ $?

paq8kx_v7.exe: paq8kx_v7/paq8kx_v7.cpp paq8kx_v7/paq7asm.o
	${CC} -o $@ $?

paq8l.exe: paq8l/paq8l.cpp paq8l/paq7asm.o
	${CC} -o $@ $?

paq8m.exe: paq8m/paq8m.cpp paq8m/paq7asm.o
	${CC} -o $@ $?

paq8n.exe: paq8n/paq8n.cpp paq8n/paq7asm.o
	${CC} -o $@ $?

paq8o.exe: paq8o/paq8o.cpp paq8o/paq7asm.o
	${CC} -o $@ $?

paq8o10t.exe: paq8o10t/paq8o10t.cpp paq8o10t/paq7asm.o
	${CC} -o $@ $?

paq8o2.exe: paq8o2/paq8o.cpp paq8o2/paq7asm.o
	${CC} -o $@ $?

paq8o3.exe: paq8o3/paq8o3.cpp paq8o3/paq7asm.o
	${CC} -o $@ $?

paq8o4v2.exe: paq8o4v2/paq8o4.cpp paq8o4v2/paq7asm.o
	${CC} -o $@ $?

paq8o5.exe: paq8o5/paq8o5.cpp paq8o5/paq7asm.o
	${CC} -o $@ $?

paq8o6.exe: paq8o6/paq8o6.cpp paq8o6/paq7asm.o
	${CC} -o $@ $?

paq8o7.exe: paq8o7/paq8o7.cpp paq8o7/paq7asm.o
	${CC} -o $@ $?

paq8o8.exe: paq8o8/paq8o8.cpp paq8o8/paq7asm.o
	${CC} -o $@ $?

#paq8o8pre.exe: paq8o8pre/paq8o8pre.cpp paq8o8pre/PAQ7ASM.asm
#	${CC} -o $@ $?

paq8o9.exe: paq8o9/paq8o9.cpp paq8o9/paq7asm.o
	${CC} -o $@ $?

paq8p.exe: paq8p/paq8p.cpp paq8p/paq7asm.o
	${CC} -o $@ $?

#paq8pxpre.exe: paq8pxpre/paq8pxpre.cpp paq8pxpre/PAQ7ASM.o
#	${CC} -o $@ $?

paq8px_v1.exe: paq8px_v1/paq8px.cpp paq8px_v1/paq7asm.o
	${CC} -o $@ $?

paq8px_v44.exe: paq8px_v44/paq8px.cpp paq8px_v44/paq7asm.o
	${CC} -o $@ $?

paq8px_v67.exe: paq8px_v67/paq8px.cpp paq8px_v67/paq7asm.o
	${CC} -o $@ $?

paq8px_v68p3.exe: paq8px_v68p3/paq8px_v68p3.cpp paq8px_v68p3/paq7asm.o
	${CC} -o $@ $?

paq8px_v68e.exe: paq8px_v68e/paq8px_v68e.cpp paq8px_v68e/paq7asm.o
	${CC} -o $@ $?

paq8px_v9.exe: paq8px_v9/paq8px.cpp paq8px_v9/paq7asm.o
	${CC} -o $@ $?

.PHONY: all clean

