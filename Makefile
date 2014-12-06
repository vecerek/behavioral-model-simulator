# Predmet: IMS ( Modelovani a simulace )
# Autor: Lelkes Oliver ( xlelke00 )
# Datum: 21.11.2012
# Popis: Makefile pre Projekt: Implementace diskretniho simulatoru pro modelovani na zaklade udalosti

# prekladac 
CC=g++

# parametry prekladace
CFLAGS= -Wall -Wextra -pedantic

all:
	$(CC) $(CFLAGS) vlek.cpp calendar.cpp random.cpp ims.cpp output.cpp stats.cpp -o vlek
	$(CC) $(CFLAGS) ucebna.cpp calendar.cpp random.cpp ims.cpp output.cpp stats.cpp -o ucebna

clean:
	rm -r vlek ucebna
run: vlek ucebna
	@./vlek
	@./ucebna

g++ -std=c++0x -Wall -Wextra -pedantic calendar.cpp -o calendar