
all: run testprog

run: run.c
	gcc -o run run.c

testprog: testprog.c
	gcc -o testprog -static testprog.c

clean:
	rm run testprog
