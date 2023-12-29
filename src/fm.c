#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <curses.h>

#include <DyArray.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))

typedef DyArray(char *) StringArray;

typedef struct {
    int pos;
    int end;
} Pos;

WINDOW *left, *right;

int max_x, max_y;

Pos cursor;
int movement = 0;

bool quit = false;

StringArray files, preview;
char *selected/*file_name*/ = NULL;

bool is_dir(char *path)
{
    struct stat statbuf;
    if (stat(path, &statbuf))
        return false;
    return S_ISDIR(statbuf.st_mode);
}

void stra_sort(StringArray *array)
{
    char *tmp = NULL;
    for (size_t i = 0; i < array->size; i++)
    {
        for (size_t j = 0; j < array->size; j++)
        {
            if (strcmp(array->data[i], array->data[j]) < 0)
            {
                tmp = array->data[i];
                array->data[i] = array->data[j];
                array->data[j] = tmp;
            }
        }
    }
}

void fill_space(WINDOW *win, int y, int x, int size)
{
    for (int j = x; j < size; ++j)
        mvwaddch(win, y, j, ' ');
}

void print_dirs(WINDOW *win, const StringArray *array)
{
    size_t width = getmaxx(win)-2;
    int count = 1;

    //skipping first one as it is "." the directory itself
    for (size_t i = 0; i < array->size; ++i)
    {
        if (cursor.pos == i)
        {
            wattron(win, COLOR_PAIR(1));
            selected = array->data[i];
        }
        count++;
        size_t len = strlen(array->data[i]);
        len = MIN(width, len);

        mvwaddstr(win, i-movement+1, 1, array->data[i]);
        fill_space(win, i-movement+1, len+1, width+1);

        if (cursor.pos == i)
            wattroff(win, COLOR_PAIR(1));
    }
    cursor.end = count-2;
}

void create_windows(void)
{
    left = subwin(stdscr, max_y, max_x/2, 0, 0);
    right = subwin(stdscr, max_y, max_x/2, 0, max_x/2);
}

void delete_windows(void)
{
    delwin(left);
    delwin(right);
}

void draw_borders(void)
{
    box(left, ACS_VLINE, ACS_HLINE);
    box(right, ACS_VLINE, ACS_HLINE);
    mvprintw(max_y-1, 0, "selected = %d-%d", cursor.pos, cursor.end);
    //mvprintw(max_y-1, 0, "file = %s", selected);
}

void read_dir(const char *dir_path, WINDOW *win)
{
    DIR *dir = opendir(dir_path);
    struct dirent *ent = readdir(dir);

    while (ent != NULL)
    {
        da_append(char *, files, ent->d_name);
        ent = readdir(dir);
    }
    stra_sort(&files);
    print_dirs(win, &files);
    files.size = 0;
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

    clear();
}

int main(int argc, char **argv)
{
    if (argc > 2)
    {
        fprintf(stderr, "Usage: fm [dir]\n");
        return 1;
    }

    initscr();
    raw();
    curs_set(0);
    noecho();
    keypad(stdscr, true);

    start_color();

    init_pair(1, COLOR_BLACK, COLOR_WHITE);

    da_passed_init(char *, files, 50);

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        create_windows();
        input_events();
        delete_windows();
    }

    da_destroy(files);
    endwin();
    return 0;
}
