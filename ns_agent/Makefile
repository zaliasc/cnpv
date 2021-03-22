all: myopen.c
	gcc -c -g -O3 -Wall -fPIC myopen.c
	gcc -g -shared -o myopen.so myopen.o -ldl

test: test.c
	gcc -g test.c -o test
# debug: socket.c indexer.c
# 	gcc -c -g -DSLIM_DEBUG -O3 -Wall -fPIC preload.c indexer.c
# 	gcc -g -shared -o SlimSocket.so socket.o indexer.o -ldl

run:
	LD_PRELOAD=./myopen.so ./test
	

clean:
	rm -f *.so *.o