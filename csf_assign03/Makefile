# Makefile for CSF Assignment 3

CC = g++
CFLAGS = -std=c++11 -g -Wall -Wextra -pedantic

EXES = main

csim : main.o cache.o set.o slot.o
	$(CC) -o csim main.o cache.o set.o slot.o

main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

cache.o : cache.cpp set.o
	$(CC) $(CFLAGS) -c cache.cpp

set.o : set.cpp slot.o
	$(CC) $(CFLAGS) -c set.cpp

slot.o : slot.cpp
	$(CC) $(CFLAGS) -c slot.cpp

clean :
	rm -f *.o $(EXES)