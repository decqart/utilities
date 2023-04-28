CC=cc

CFLAGS=-g -Wall -std=c99 -pedantic -O3 -D_DEFAULT_SOURCE
LDFLAGS=-lcurses

O=bin
S=src

default:		\
	dirs		\
	clear		\
	snake		\
	fm			\
	bed			\
	grep		\
	whoami		\
	yes

clean:
	rm -rf $(O)/*

dirs:
	mkdir -p $(O)

snake: $(S)/snake.c
	$(CC) $(CFLAGS) $(S)/snake.c \
	-o $(O)/snake $(LDFLAGS)

fm: $(S)/fm.c
	$(CC) $(CFLAGS) $(S)/fm.c \
	-o $(O)/fm $(LDFLAGS)

bed: $(S)/bed.c
	$(CC) $(CFLAGS) $(S)/bed.c \
	-o $(O)/bed $(LDFLAGS)

grep: $(S)/grep.c
	$(CC) $(CFLAGS) $(S)/grep.c \
	-o $(O)/grep

whoami: $(S)/whoami.c
	$(CC) $(CFLAGS) $(S)/whoami.c \
	-o $(O)/whoami

yes: $(S)/yes.c
	$(CC) $(CFLAGS) $(S)/yes.c \
	-o $(O)/yes

clear: $(S)/clear.c
	$(CC) $(CFLAGS) $(S)/clear.c \
	-o $(O)/clear
