#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <curses.h>

#include "StrArray.h"

#define MIN(a, b) ((a) < (b) ? (a) : (b))

WINDOW *left, *right;

int max_x, max_y;

typedef struct {
    int pos;
    int end;
} Pos;

Pos cursor;

bool quit = false;

StrArray preview;
char *selected/*file_name*/ = NULL;
char *err = "None";

bool is_dir(char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void print_dirs(WINDOW *win, StrArray array)
{
    int i;
    //skipping first one as it is "." the directory itself
    for (i = 1; array.value[i] != NULL; ++i)
    {
        if (cursor.pos == i-1)
        {
            wattron(win, COLOR_PAIR(1));
            selected = array.value[i];
        }
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
        if (cursor.pos == i-1)
            wattroff(win, COLOR_PAIR(1));
    }
    cursor.end = i-2;
}

void create_windows(void)
{
    left = subwin(stdscr, max_y, max_x/2, 0, 0);
    right = subwin(stdscr, max_y, max_x/2, 0, max_x/2);
}

void draw_borders(void)
{
    box(left, ACS_VLINE, ACS_HLINE);
    box(right, ACS_VLINE, ACS_HLINE);
    //mvprintw(max_y-1, 0, "selected = %d-%d", cursor.pos, cursor.end);
    mvprintw(max_y-1, 0, "error = %s", selected);
    wrefresh(left);
    wrefresh(right);
}

void read_dir(const char *dir_path, WINDOW *win)
{
    DIR *dir = opendir(dir_path);
    struct dirent *ent = readdir(dir);

    StrArray files = stra_init();

    while (ent != NULL)
    {
        stra_append(&files, ent->d_name);
        ent = readdir(dir);
    }
    stra_sort(&files);
    print_dirs(win, files);
    stra_destroy(&files);
    closedir(dir);
}

void input_events(void)
{
    //TODO: make cwd better
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    setenv("PWD", cwd, 1);
    read_dir(cwd, left);
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
        if (is_dir(selected))
        {
            chdir(selected);
            cursor.pos = 0;
        }
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
