CC=gcc
MOVE=mv

CFLAGS=-O -c
LFLAGS=-lcrypto -lz -lm -lpng

FILE=Main.c Crypto.c Png.c
OFILE=obj/Main.o obj/Crypto.o obj/Png.o

VALGRIND=valgrind
VFLAGS= --leak-check=yes --track-origins=yes -v
OUT=./a.out

all: 
	$(CC)  $(CFLAGS) $(FILE) $(LFLAGS)
	$(MOVE) *.o obj
	$(CC)  $(OFILE) $(LFLAGS)

clean:
	rm -rf *.o
	rm -rf *.out
	rm -rf obj/*.o

valgrind:
	$(VALGRIND) $(VFLAGS) $(OUT)