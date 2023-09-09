CC=cc
LD=gold

CFLAGS=-g -Wall -Wextra -std=c99 -pedantic -Iinclude -O3 -D_DEFAULT_SOURCE
LDFLAGS=-fuse-ld=$(LD)

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
	$(CC) $(CFLAGS) $< -o $(O)/snake $(LDFLAGS) -lcurses

fm: $(S)/fm.c
	$(CC) $(CFLAGS) $< -o $(O)/fm $(LDFLAGS) -lcurses

ls: $(S)/ls.c
	$(CC) $(CFLAGS) $< -o $(O)/ls $(LDFLAGS)

grep: $(S)/grep.c
	$(CC) $(CFLAGS) $< -o $(O)/grep $(LDFLAGS)

uname: $(S)/uname.c
	$(CC) $(CFLAGS) $< -o $(O)/uname $(LDFLAGS)

whoami: $(S)/whoami.c
	$(CC) $(CFLAGS) $< -o $(O)/whoami $(LDFLAGS)

wc: $(S)/wc.c
	$(CC) $(CFLAGS) $< -o $(O)/wc $(LDFLAGS)

yes: $(S)/yes.c
	$(CC) $(CFLAGS) $< -o $(O)/yes $(LDFLAGS)

clear: $(S)/clear.c
	$(CC) $(CFLAGS) $< -o $(O)/clear $(LDFLAGS)

cat: $(S)/cat.c
	$(CC) $(CFLAGS) $< -o $(O)/cat $(LDFLAGS)
