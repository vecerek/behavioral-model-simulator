# Makefile for IMS

CC=g++

CFLAGS= -std=c++0x -Wall -Wextra -pedantic

all:
	$(CC) $(CFLAGS) vlek.cpp calendar.cpp functions.cpp utility.cpp histogram.cpp -o vlek
	$(CC) $(CFLAGS) ucebna.cpp calendar.cpp functions.cpp utility.cpp histogram.cpp -o ucebna

clean:
	rm -r vlek ucebna

run: vlek ucebna
	@./vlek
	@./ucebna