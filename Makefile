CC=cc

CFLAGS=-ggdb -Wall -std=c99 -pedantic -O3 -D_DEFAULT_SOURCE
LDFLAGS=-lcurses

O=bin
S=src

default:	\
	dirs	\
	clear	\
	snake	\
	fm		\
	bed		\
	grep	\
	whoami	\
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

bed: $(S)/bed.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/bed $(LDFLAGS)

grep: $(S)/grep.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/grep

whoami: $(S)/whoami.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/whoami

yes: $(S)/yes.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/yes

clear: $(S)/clear.c
	$(CC) $(CFLAGS) $< \
	-o $(O)/clear
