CC=cc

CFLAGS=-g -Wall -std=c99 -pedantic -O3
LDFLAGS=-lcurses

O=bin

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

snake: snake.c
	$(CC) $(CFLAGS) snake.c \
	-o $(O)/snake $(LDFLAGS)

fm: fm.c
	$(CC) $(CFLAGS) fm.c \
	-o $(O)/fm $(LDFLAGS)

bed: bed.c
	$(CC) $(CFLAGS) bed.c \
	-o $(O)/bed $(LDFLAGS)

grep: grep.c
	$(CC) $(CFLAGS) -D_DEFAULT_SOURCE grep.c \
	-o $(O)/grep

whoami: whoami.c
	$(CC) $(CFLAGS) whoami.c \
	-o $(O)/whoami

yes: yes.c
	$(CC) $(CFLAGS) yes.c \
	-o $(O)/yes

clear: clear.c
	$(CC) $(CFLAGS) clear.c \
	-o $(O)/clear
