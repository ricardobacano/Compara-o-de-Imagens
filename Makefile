CC = gcc
CFLAGS = -Wall -O2
OBJ = main.o lbp.o pgm.o 
EXEC = LBP

all: $(EXEC)

$(EXEC): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXEC) $(OBJ) -lm

main.o: main.c lbp.h pgm.h 
	$(CC) $(CFLAGS) -c main.c

lbp.o: lbp.c lbp.h pgm.h
	$(CC) $(CFLAGS) -c lbp.c

pgm.o: pgm.c pgm.h
	$(CC) $(CFLAGS) -c pgm.c

clean:
	rm -f *.o $(EXEC)
