CFLAGS=-std=c11 -g -static

cc: main.c

test: cc
					./test.sh

clean: 
					rm -f cc *.o *~ tmp*

.PHONY: test clean