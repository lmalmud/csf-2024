# Makefile for CSF Assignment 3

CC = g++
CFLAGS = -std=c++11 -g -Wall -Wextra -pedantic

EXES = main

csim : main.o
	$(CC) -o csim main.o 

main.o : main.cpp
	$(CC) $(CFLAGS) -c main.cpp

clean :
	rm -f *.o $(EXES)