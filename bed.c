// non functional for now
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

int max_y = 0, max_x = 0;
int cursor_y = 0, cursor_x = 0;
int submax_y, submax_x;
bool quit = false;

typedef struct {
    size_t len;
} Line;

typedef struct {
    char *content;
    size_t size;
    Line *line;
} Text;

Text text;

void init_text(Text *buf, char *cont)
{
    size_t line_count = 0;
    buf->content = malloc(strlen(cont) * sizeof(char));
    buf->content = strcat(buf->content, cont);
    buf->size = strlen(cont);
    for (int i = 0; i < buf->size; ++i)
    {
        if (buf->content[i] == '\n')
            line_count++;
    }
    buf->line = malloc(line_count * sizeof(Line));
}

void insert_char(Text *buf, int ch)
{
    buf->content[buf->size++] = ch;
    buf->content = realloc(buf->content, buf->size * sizeof(char)+1);
}

void free_text(Text *buf)
{
    free(buf->content);
    buf->content = NULL;
    buf->size = 0;
}

void key_events(void)
{   
    int ch = getch();
    switch (ch)
    {
    case '\n':
        insert_char(&text, '\n');
        cursor_x = 0;
        cursor_y++;
        break;
    case KEY_UP:
        break;
    case KEY_LEFT:
        if (cursor_x == 0)
            cursor_y--;
        if (cursor_x != 0)
            cursor_x--;
        break;  
    case KEY_DOWN:
        cursor_y++;
        break;
    case KEY_RIGHT:
        cursor_x++;
        break;
    case 127:
    case KEY_BACKSPACE:
        if (cursor_x == 0 && cursor_y != 0)
        {
            cursor_y--;
        }
        if (cursor_x != 0)
            cursor_x--;
        clear();
        break;
    case 27:
        quit = true;
        break;
    default:
        insert_char(&text, ch);
        cursor_x++;
        break;
    }
}

int main(void)
{
    int height = max_y, width = max_x;
    initscr();
    raw();
    curs_set(1);
    noecho();
    keypad(stdscr, true);

    init_text(&text, "");

    int ncount = 0;

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        mvprintw(0, 0, text.content);
        mvprintw(max_y-1, 0, "size = %ld", text.size);
        ncount = 0;
        for (int i = 0; i < text.size; ++i)
        {
            if (text.content[i] == '\n')
                ncount++;
        }
        mvprintw(max_y-1, 10, "'\\n' count = %d", ncount);
        move(cursor_y, cursor_x);
        key_events();

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

