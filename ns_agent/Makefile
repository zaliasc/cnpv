cc = gcc
obj = agent.o check.o config.o hashmap.o json.o cache.o lru.o
INC=-I include/
# CFLAGS := -g -O0 -Wall -fPIC -Wno-strict-aliasing -Wno-unused-variable
# CFLAGS_AGENT := -g -shared
CFLAGS := -O3 -Wall -fPIC -Wno-strict-aliasing -Wno-unused-variable
CFLAGS_AGENT := -shared
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

cache.o : cache.c
	$(cc) -c $(CFLAGS) $(INC) cache.c	

lru.o : lru.c
	$(cc) -c $(CFLAGS) $(INC) lru.c	

hashmap.o : src/hashmap.c
	$(cc) -c $(CFLAGS) $(INC) src/hashmap.c                                  

json.o : src/json.c
	$(cc) -c $(CFLAGS) $(INC) src/json.c


# test
open : test/test_open.c
	gcc -g test/test_open.c -o open
	# LD_PRELOAD=./agent.so ./open

openat : test/test_openat.c
	gcc -g test/test_openat.c -o openat
	# LD_PRELOAD=./agent.so ./openat

fopen : test/test_fopen.c
	gcc -g test/test_fopen.c -o fopen
	# LD_PRELOAD=./agent.so ./open

cache : test/test_cache. lru.c cache.c
	gcc test/test_cache.c lru.c cache.c -o cache
	# LD_PRELOAD=./agent.so ./cache

clean :
	rm -rf *.o *.out *.so open openat fopen cache *.log

# set environment LD_PRELOAD ./agent.so
debug: all
	# gcc -g test/test.c -o test
	gdb -iex "set exec-wrapper env LD_PRELOAD=./agent.so" ./test
	# set exec-wrapper env 'LD_PRELOAD=agent.so'

preload: test/preload.c
	gcc -fPIC -shared -o preload.so test/preload.c -ldl