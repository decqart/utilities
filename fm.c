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
int movement = 0;

bool quit = false;

StrArray files;
StrArray preview;
char *selected/*file_name*/ = NULL;

bool is_dir(char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void fill_space(WINDOW *win, int y, int x, int size)
{
    for (int j = x; j < size; ++j)
        mvwaddch(win, y, j, ' ');
}

void print_dirs(WINDOW *win, StrArray array)
{
    int width = getmaxx(win)-2;
    int count = 1;

    //skipping first one as it is "." the directory itself
    for (int i = 1; array.value[i] != NULL; ++i)
    {
        if (cursor.pos == i-1)
        {
            wattron(win, COLOR_PAIR(1));
            selected = array.value[i];
        }
        count++;
        size_t len = strlen(array.value[i]);
        len = MIN(width, len);

        mvwaddstr(win, i-movement, 1, array.value[i]);
        fill_space(win, i-movement, len+1, width+1);

        if (cursor.pos == i-1)
            wattroff(win, COLOR_PAIR(1));
    }
    cursor.end = count-2;
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
    mvprintw(max_y-1, 0, "selected = %d-%d", cursor.pos, cursor.end);
    //mvprintw(max_y-1, 0, "file = %s", selected);
    wrefresh(left);
    wrefresh(right);
}

void read_dir(const char *dir_path, WINDOW *win)
{
    DIR *dir = opendir(dir_path);
    struct dirent *ent = readdir(dir);

    while (ent != NULL)
    {
        stra_append(&files, ent->d_name);
        ent = readdir(dir);
    }
    stra_sort(&files);
    print_dirs(win, files);
    stra_empty(&files);
    closedir(dir);
}

void input_events(void)
{
    //TODO: make cwd better
    char cwd[256];
    getcwd(cwd, sizeof(cwd));
    read_dir(cwd, left);
    draw_borders();

    switch(getch())
    {
    case KEY_UP:
        if (cursor.pos != 0)
        {
            cursor.pos--;
            if (cursor.pos < movement)
                movement--;
        }
        break;
    case KEY_LEFT:
        break;
    case KEY_DOWN:
        if (cursor.pos != cursor.end)
        {
            cursor.pos++;
            if (cursor.pos > max_y-3+movement)
                movement++;
        }
        break;
    case KEY_RIGHT:
        break;
    case '\n':
        if (is_dir(selected))
        {
            chdir(selected);
            cursor.pos = 0;
            movement = 0;
        }
        break;
    case 'q':
        quit = true;
        break;
    default:
        break;
    }

    wclear(left);
    wclear(right);
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

    files = stra_init();

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

    stra_destroy(&files);
    endwin();
    return 0;
}
