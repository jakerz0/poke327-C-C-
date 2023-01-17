all: clean heap.o assignment110

assignment110: assignment1_10.cpp
	g++ -Wall -Werror -g heap.o assignment1_10.cpp -lm  -lncursesw  -o assignment1_10

heap.o: heap.c heap.h
	g++ -Wall -Werror -g heap.c -c

clean:
	rm -f *.o *~ 