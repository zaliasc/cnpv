all: agent.c check.c config.c hashmap.c
	gcc -c -g -O3 -Wall -fPIC agent.c check.c config.c hashmap.c
	gcc -g -shared -o agent.so agent.o check.o config.o hashmap.o -ldl -ljansson
	rm -rf *.o

test: test.c
	gcc -g test.c -o test
	LD_PRELOAD=./agent.so ./test
	
# set environment LD_PRELOAD ./agent.so
debug: all
	gcc -g test.c -o test

clean:
	rm -f *.so *.o *.out

debug1:
	gcc -g agent.c check.c config.c hashmap.c test.c -o test -ljansson -ldl