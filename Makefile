CC=cc

CFLAGS=-g -Wall -std=c99 -pedantic -Iinclude -O3 -D_DEFAULT_SOURCE
LDFLAGS=-lcurses

O=bin
S=src

default:	\
	dirs	\
	cat		\
	clear	\
	ls		\
	snake	\
	fm		\
	grep	\
	uname	\
	whoami	\
	wc		\
	yes

clean:
	rm -rf $(O)/*

dirs:
	@mkdir -p $(O)

snake: $(S)/snake.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/snake $(LDFLAGS)

fm: $(S)/fm.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/fm $(LDFLAGS)

ls: $(S)/ls.c
	$(CC) $(CFLAGS) $< -o $(O)/ls

grep: $(S)/grep.c
	$(CC) $(CFLAGS) $< -o $(O)/grep

uname: $(S)/uname.c
	$(CC) $(CFLAGS) $< -o $(O)/uname

whoami: $(S)/whoami.c
	$(CC) $(CFLAGS) $< -o $(O)/whoami

wc: $(S)/wc.c
	$(CC) $(CFLAGS) $< -o $(O)/wc

yes: $(S)/yes.c
	$(CC) $(CFLAGS) $< -o $(O)/yes

clear: $(S)/clear.c
	$(CC) $(CFLAGS) $< -o $(O)/clear

cat: $(S)/cat.c
	$(CC) $(CFLAGS) $< -o $(O)/cat
