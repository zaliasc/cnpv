all: agent.c check.c config.c hashmap.c
	gcc -c -g -O3 -Wall -fPIC agent.c check.c config.c hashmap.c json.c 
	gcc -g -shared -o agent.so agent.o check.o config.o hashmap.o json.o -ldl -lm
	rm -rf *.o

test:
	gcc -g test.c -o test
	LD_PRELOAD=./agent.so ./test
	
# set environment LD_PRELOAD ./agent.so
debug: all
	gcc -g test.c -o test
	gdb -iex "set exec-wrapper env LD_PRELOAD=./agent.so" ./test
	# set exec-wrapper env 'LD_PRELOAD=agent.so'

clean:
	rm -f *.so *.o *.out

debug1:
	gcc -g agent.c check.c config.c hashmap.c test.c -o test -ljansson -ldl