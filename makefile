CC=gcc

# Every warning possible, safety first
CFLAGS=-g -D_GNU_SOURCE=1 -D_REENTRANT -Wall -W -Wfloat-equal -Wundef -Wshadow -Wpointer-arith -Wstrict-prototypes -Wswitch-default -Wconversion -Wunreachable-code

IMAGE_FLAGS=-lSDL2_image
SFLAGS=-lSDL2
SOURCE=main.c
BINARIES=main

all: $(BINARIES)

main: $(SOURCE)
	$(CC) $(SOURCE) -o $(BINARIES) $(CFLAGS) $(SFLAGS) $(IMAGE_FLAGS) 

clean:
	rm -rf *.o *.exe *.bak *.c~ $(BINARIES) core a.out
