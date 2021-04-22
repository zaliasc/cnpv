cc = gcc
obj = agent.o check.o config.o hashmap.o json.o
INC=-I include/
CFLAGS := -g -O3 -Wall -fPIC -Wno-strict-aliasing -Wno-unused-variable
CFLAGS_AGENT := -g -shared
LD_FLAG := -ldl -lm

all : $(obj)
	$(cc) $(CFLAGS_AGENT) -o agent.so $(obj) $(LD_FLAG)
	rm -rf $(obj)

agent.o : agent.c
	$(cc) -c $(CFLAGS) $(INC) agent.c

check.o : check.c
	$(cc) -c $(CFLAGS) $(INC) check.c

config.o : config.c
	$(cc) -c $(CFLAGS) $(INC) config.c	

hashmap.o : src/hashmap.c
	$(cc) -c $(CFLAGS) $(INC) src/hashmap.c                                  

json.o : src/json.c
	$(cc) -c $(CFLAGS) $(INC) src/json.c


# test
open : open.c
	gcc -g open.c -o open
	# LD_PRELOAD=./agent.so ./open

openat : openat.c
	gcc -g openat.c -o openat
	# LD_PRELOAD=./agent.so ./openat

fopen : fopen.c
	gcc -g fopen.c -o fopen
	# LD_PRELOAD=./agent.so ./open

clean :
	rm -rf *.o *.out *.so open openat fopen 

# set environment LD_PRELOAD ./agent.so
debug: all
	# gcc -g test.c -o test
	gdb -iex "set exec-wrapper env LD_PRELOAD=./agent.so" ./fopen
	# set exec-wrapper env 'LD_PRELOAD=agent.so'

preload: preload.c
	gcc -fPIC -shared -o preload.so preload.c -ldl