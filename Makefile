
all: run testprog

run: run.c
	gcc -DHAVE_CONFIG_H -I. -Iudis86 -Iudis86/libudis86 -g -O2 -Wall -MT run-run.o -MD -MP -c -o run-run.o run.c
	libtool --tag=CC --mode=link gcc -Iudis86/libudis86 -Iudis86 -g -O2 -Wall -o run run-run.o udis86/libudis86/libudis86.la

testprog: testprog.c
	gcc -o testprog -static testprog.c

clean:
	rm run testprog
