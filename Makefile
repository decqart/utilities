CC=cc

CFLAGS=-g -Wall -std=c99 -pedantic
LDFLAGS=-lcurses

O=bin

default:		\
	clear		\
	snake		\
	calculator	\
	filemanager	\
	bed			\
	whoami		\
	yes

clean:
	rm -rf $(O)/*

snake: snake.c
	$(CC) $(CFLAGS) snake.c \
	-o $(O)/snake $(LDFLAGS)

calculator: calculator.c
	$(CC) $(CFLAGS) calculator.c \
	-o $(O)/calc $(LDFLAGS)

filemanager: fm.c
	$(CC) $(CFLAGS) fm.c \
	-o $(O)/fm $(LDFLAGS)

bed: bed.c
	$(CC) $(CFLAGS) bed.c \
	-o $(O)/bed $(LDFLAGS)

whoami: whoami.c
	$(CC) $(CFLAGS) whoami.c \
	-o $(O)/whoami

yes: yes.c
	$(CC) $(CFLAGS) yes.c \
	-o $(O)/yes

clear: clear.c
	$(CC) $(CFLAGS) clear.c \
	-o $(O)/clear
