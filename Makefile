CC=cc
LD=gold

CFLAGS=-g -Wall -Wextra -std=c99 -pedantic -Iinclude -O3 -D_DEFAULT_SOURCE
LDFLAGS=-fuse-ld=$(LD)

O=bin
S=src

default:			\
	dirs			\
	cat				\
	clear			\
	ls				\
	grep			\
	uname			\
	whoami			\
	wc				\
	yes				\
	snake-curses	\
	fm-curses

clean:
	rm -rf $(O)/*

dirs:
	@mkdir -p $(O)

%: $(S)/%.c
	$(CC) $(CFLAGS) $< -o $(O)/$@ $(LDFLAGS)

%-curses: $(S)/%.c
	$(CC) $(CFLAGS) $< -o $(O)/$(patsubst %-curses,%,$@) $(LDFLAGS) -lcurses
