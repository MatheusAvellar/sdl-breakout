CC=gcc

CFLAGS=-D_GNU_SOURCE=1 -D_REENTRANT -Wall -W -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wstrict-prototypes -Wswitch-default -Wunreachable-code -pedantic #-Wconversion 
SDLFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer

all: main

main: main.o defs.o globals.o
	$(CC) main.o defs.o globals.o -o main $(CFLAGS) $(SDLFLAGS)

main.o: main.c
	$(CC) -c main.c $(CFLAGS) $(SDLFLAGS)

defs.o: defs.c
	$(CC) -c defs.c $(CFLAGS) $(SDLFLAGS)

globals.o: globals.c
	$(CC) -c globals.c $(CFLAGS) $(SDLFLAGS)

clean:
	rm -rf *.o *.exe *.bak *.c~ main core a.out
