
all: run testprog hello

run: run.c
	gcc -DHAVE_CONFIG_H -I. -Iudis86 -Iudis86/libudis86 -g -O2 -Wall -MT run.o -MD -MP -c -o run.o $<
	libtool --tag=CC --mode=link gcc -Iudis86/libudis86 -Iudis86 -g -O2 -Wall -o $@ run.o udis86/libudis86/libudis86.la

testprog: testprog.c
	gcc -o testprog -static $<

hello: hello.o
	ld -o $@ $<

hello.o: hello.asm
	yasm -f elf64 -g dwarf2 $<

clean:
	rm -f run run.o run.d testprog hello hello.o
