CC=gcc

CFLAGS=-g -D_GNU_SOURCE=1 -D_REENTRANT -Wall -W -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wstrict-prototypes -Wswitch-default -Wunreachable-code -pedantic #-Wconversion 

SDLFLAGS=-lSDL2 -lSDL2_image -lSDL2_mixer
SOURCE=main.c
BINARIES=main

all: $(BINARIES)

main: $(SOURCE)
	$(CC) $(SOURCE) -o $(BINARIES) $(CFLAGS) $(SDLFLAGS)

clean:
	rm -rf *.o *.exe *.bak *.c~ $(BINARIES) core a.out
