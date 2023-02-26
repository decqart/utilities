#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <dirent.h>
#include <curses.h>

int max_x, max_y;

WINDOW *left, *middle, *right;

int pos = 0;
int max_pos = 0;

bool quit = false;

void nowrap_mvwprintw(WINDOW *win, int y, int x, char *str)
{
    if (str == NULL) return;
    int width, height;
    getmaxyx(win, height, width);
    char *dstr = strndup(str, width-2);
    char *end = &dstr[strlen(dstr)];
    memset(end, ' ', width-2-strlen(dstr));
    (void) height;

    mvwprintw(win, y, x, dstr);
    free(dstr);
}

void create_windows(void)
{
    left = subwin(stdscr, max_y, max_x/3, 0, 0);
    middle = subwin(stdscr, max_y, max_x/3, 0, max_x/3);
    right = subwin(stdscr, max_y, max_x/3, 0, max_x*2/3);
}

void draw_borders(void)
{
    box(left, ACS_VLINE, ACS_HLINE);
    box(middle, ACS_VLINE, ACS_HLINE);
    box(right, ACS_VLINE, ACS_HLINE);
    mvprintw(max_y-1, 0, "selected = %d-%d", pos, max_pos);
    wrefresh(left);
    wrefresh(middle);
    wrefresh(right);
}

void read_dir(const char *dir_path, WINDOW *win)
{
    DIR *dir = opendir(dir_path);
    struct dirent *ent = readdir(dir);

    size_t count = 1;
    while (ent != NULL)
    {
        if (pos == count-1)
            wattron(win, COLOR_PAIR(1));
        nowrap_mvwprintw(win, count, 1, ent->d_name);
        if (pos == count-1)
            wattroff(win, COLOR_PAIR(1));
        count++;
        ent = readdir(dir);
    }
    max_pos = count-2;
    closedir(dir);
}

void input_events(void)
{
    read_dir(getenv("PWD"), left);
    draw_borders();

    switch(getch())
    {
    case KEY_UP:
        if (pos != 0) pos--;
        break;
    case KEY_LEFT:
        break;
    case KEY_DOWN:
        if (pos != max_pos) pos++;
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
