#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <curses.h>

#include "StrArray.h"

//void read_dir(const char *dir_path, WINDOW *win, StrArray str_array);

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int max_x, max_y;

WINDOW *left, *middle;

typedef struct {
    int pos;
    int end;
} Pos;

Pos cursor;

bool quit = false;

StrArray files;
StrArray preview;
char *selected/*file_name*/ = NULL;
char *err = "None";
int ierr = 0;

bool is_dir(char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void print_dir(WINDOW *win, StrArray array)
{
    int i;
    for (i = 1; array.value[i] != NULL; ++i)
    {
        //if (is_dir(array.value[i]))
        //read_dir(array.value[i], middle, preview);
        if (cursor.pos == i-1)
            wattron(win, COLOR_PAIR(1));
        size_t len = strlen(array.value[i]);
        int width = getmaxx(win)-2;
        len = MIN(width, len);
        char *dstr = malloc(width+1*sizeof(char));
        strncpy(dstr, array.value[i], len);
        char *end = &dstr[len];
        //last bytes are filled with whitespace to take use of color
        memset(end, ' ', width-len);
        dstr[width] = '\0';

        mvwaddstr(win, i, 1, dstr);
        free(dstr);
        ierr = width;
        if (cursor.pos == i-1)
            wattroff(win, COLOR_PAIR(1));
    }
    cursor.end = i-2;
}

void create_windows(void)
{
    left = subwin(stdscr, max_y, max_x/3, 0, 0);
    middle = subwin(stdscr, max_y, max_x/3, 0, max_x/3);
}

void draw_borders(void)
{
    box(left, ACS_VLINE, ACS_HLINE);
    box(middle, ACS_VLINE, ACS_HLINE);
    mvprintw(max_y-1, 0, "selected = %d-%d", cursor.pos, cursor.end);
    //mvprintw(max_y-1, 0, "error = %s-%d", err, ierr);
    wrefresh(left);
    wrefresh(middle);
}

void read_dir(const char *dir_path, WINDOW *win)
{
    DIR *dir = opendir(dir_path);
    struct dirent *ent = readdir(dir);

    files = stra_init();

    while (ent != NULL)
    {
        stra_append(&files, ent->d_name);
        ent = readdir(dir);
    }
    stra_sort(&files);
    print_dir(win, files);
    stra_destroy(&files);
    closedir(dir);
}

void input_events(void)
{
    read_dir(getenv("PWD"), left);
    draw_borders();

    switch(getch())
    {
    case KEY_UP:
        if (cursor.pos != 0) cursor.pos--;
        break;
    case KEY_LEFT:
        break;
    case KEY_DOWN:
        if (cursor.pos != cursor.end) cursor.pos++;
        break;
    case KEY_RIGHT:
        break;
    case '\n':
        break;
    case 'q':
        quit = true;
        break;
    default:
        break;
    }
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        puts("usage: fm [dir]");
        return 1;
    }
    int height = 0, width = 0;
    initscr();
    raw();
    curs_set(0);
    noecho();
    keypad(stdscr, true);

    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        create_windows();
        input_events();

        if (max_x != width || max_y != height)
        {
            width = max_x;
            height = max_y;
            clear();
        }
    }

    endwin();
    return 0;
}
