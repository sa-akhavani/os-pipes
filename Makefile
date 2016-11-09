CC=g++
all: a.out
a.out: main.o
	$(CC) main.o -o a.out
main.o: main.cpp main.h
	$(CC) -c main.cpp
clean:
	rm *.o
