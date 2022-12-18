CC=cc

CFLAGS=-g -Wall -std=c99
LDFLAGS=-lcurses

O=bin

default: snake calculator

clean:
	rm -rf $(O)/*

snake: snake.c
	$(CC) $(CFLAGS) snake.c \
	-o $(O)/snake $(LDFLAGS)

calculator: calculator.c
	$(CC) $(CFLAGS) calculator.c \
	-o $(O)/calc $(LDFLAGS)

