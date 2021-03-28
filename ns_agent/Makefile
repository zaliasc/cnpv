all: agent.c
	gcc -c -g -O3 -Wall -fPIC agent.c check.c
	gcc -g -shared -o agent.so agent.o check.o -ldl

test: test.c
	gcc -g test.c

run:
	LD_PRELOAD=./agent.so ./a.out
	
clean:
	rm -f *.so *.o *.out