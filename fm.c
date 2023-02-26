#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <dirent.h>
#include <curses.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

int max_x, max_y;

WINDOW *left, *middle, *right;

typedef struct {
    int pos;
    int end;
} Pos;

Pos cursor;

bool quit = false;

char *selected/*file_name*/ = NULL;
char *err = "None";
int ierr = 0;

void alphabetic_sort(char **array)
{
    char *tmp = NULL;
    for (int i = 0; array[i]; i++)
    {
        for (int j = 0; array[j]; j++)
        {
            if (strcmp(array[i], array[j]) < 0)
            {
                tmp = array[i];
                array[i] = array[j];
                array[j] = tmp;
            }
        }
    }
}

bool is_dir(char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void print_dir(WINDOW *win, int y, int x, char *str)
{
    size_t len = strlen(str);
    int width = getmaxx(win)-2;
    len = MIN(width, len);
    char *dstr = malloc(width+1*sizeof(char));
    strncpy(dstr, str, len);
    char *end = &dstr[len];
    //last bytes are filled with whitespace to take use of color
    memset(end, ' ', width-len);
    dstr[width] = '\0';

    mvwaddstr(win, y, x, dstr);
    free(dstr);
    ierr = width;
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
    //mvprintw(max_y-1, 0, "selected = %d-%d", cursor.pos, cursor.end);
    mvprintw(max_y-1, 0, "error = %s-%d", err, ierr);
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
        if (cursor.pos == count-1)
        {
            wattron(win, COLOR_PAIR(1));
            print_dir(win, count, 1, ent->d_name);
            wattroff(win, COLOR_PAIR(1));
        }
        else
            print_dir(win, count, 1, ent->d_name);
        count++;
        ent = readdir(dir);
    }
    cursor.end = count-2;
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
