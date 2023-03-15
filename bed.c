#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>

int max_y = 0, max_x = 0;
bool quit = false;

typedef struct {
    int x;
    int y;
} Pos;

typedef struct {
    size_t len;
    size_t height;
} Line;

typedef struct {
    Line *line;
    size_t count;
    size_t used;
} Lines;

typedef struct {
    char *content;
    size_t filled;
    size_t size;
    Pos pos;
    Line *line;
    size_t line_count;
} Text;

inline size_t get_newline_count(const char *str)
{
    size_t count = 0;
    while (*str++)
        if (*str == '\n')
            count++;
    return count;
}

Text init_text(char *cont)
{
    char *buffer = NULL;
    Line *line = NULL;
    size_t filled_size = 0;
    size_t buffer_size = 0;
    size_t line_count = 0;

    if (cont != NULL)
    {
        size_t nl_count = get_newline_count(cont)+1;
        size_t cont_len = strlen(cont);
        buffer = malloc(cont_len * 2 * sizeof(char));
        memcpy(buffer, cont, cont_len+1);
        buffer_size = cont_len;

        line = malloc(nl_count * 2 * sizeof(Line));
        line_count = nl_count * 2;

        size_t line_len = 0;
        size_t line_num = 0;
        for (int i = 0; i < buffer_size; ++i)
        {
            if (buffer[i] == '\n')
            {
                line[line_num].len = line_len;
                line_num++;
                line_len = 0;
                continue;
            }
            line_len++;
        }
        filled_size = cont_len;
    }
    else
    {
        buffer = malloc(128 * sizeof(char));
        buffer[0] = '\0';
        buffer_size = 128;
        line = malloc(20 * sizeof(Line));
        line_count = 20;
        for (int i = 0; i < 20; ++i)
            line[i].len = 0;
    }

    return (Text) {
        buffer, filled_size,
        buffer_size, (Pos) { 0, 0 },
        line, line_count
    };
}

size_t get_actual_pos(Text *text)
{
    size_t pos = 0;
    for (int i = 0; i < text->pos.y; ++i)
        pos += text->line[i].len;

    return pos+text->pos.x;
}

void move_up(Text *text)
{
    if (text->pos.y != 0)
    {
        text->pos.y--;
        if (text->line[text->pos.y].len-1 < text->pos.x)
            text->pos.x = text->line[text->pos.y].len-1;
    }
}

void move_down(Text *text)
{
    //FIXIT: replace cursor based on if line has '\n'
    if (text->line[text->pos.y+1].len != 0)
    {
        text->pos.y++;
        if (text->line[text->pos.y].len-1 <= text->pos.x)
            text->pos.x = text->line[text->pos.y].len-1;
    }
}

void move_left(Text *text)
{
    if (text->pos.x == 0 && text->pos.y != 0)
    {
        text->pos.y--;
        text->pos.x = text->line[text->pos.y].len;
    }
    if (text->pos.x != 0)
        text->pos.x--;
}

void move_right(Text *text)
{
    size_t pos = get_actual_pos(text);
    if (text->content[pos] == '\n')
    {
        text->pos.y++;
        text->pos.x = 0;
    }
    else if (text->line[text->pos.y].len != text->pos.x)
        text->pos.x++;
}

void insert_char(Text *buf, char ch)
{
    buf->filled++;
    if (buf->filled == buf->size)
    {
        buf->size <<= 1;
        buf->content = realloc(buf->content, buf->size * sizeof(char));
    }
    buf->content[buf->filled] = '\0';

    size_t pos = get_actual_pos(buf);
    if (ch == '\n')
    {
        for (int i = buf->line_count; buf->pos.y+1 < i; --i)
            buf->line[i].len = buf->line[i-1].len;

        buf->line[buf->pos.y+1].len = buf->line[buf->pos.y].len - buf->pos.x;
        buf->line[buf->pos.y].len = buf->pos.x;
    }
    buf->line[buf->pos.y].len++;
    for (size_t i = buf->filled; pos < i; --i)
        buf->content[i] = buf->content[i-1];

    buf->content[pos] = ch;
}

void delete_char(Text *buf)
{
    if (buf->pos.x == 0 && buf->pos.y == 0) return;
    move_left(buf);
    buf->filled--;
    size_t pos = get_actual_pos(buf);

    if (buf->content[pos] == '\n')
    {
        buf->line[buf->pos.y].len += buf->line[buf->pos.y+1].len-1;

        for (int i = buf->pos.y+1; i < buf->line_count; ++i)
            buf->line[i].len = buf->line[i+1].len;
    }
    else
        buf->line[buf->pos.y].len--;

    for (size_t i = pos; i < buf->filled; ++i)
        buf->content[i] = buf->content[i+1];

    buf->content[buf->filled] = '\0';
}

void free_text(Text *buf)
{
    free(buf->content);
    free(buf->line);
    buf->content = NULL;
    buf->line = NULL;
    buf->size = 0;
    buf->filled = 0;
    buf->pos = (Pos) { 0, 0 };
}

void key_events(Text *text)
{
    int ch = getch();
    switch (ch)
    {
    case '\n':
        insert_char(text, '\n');
        text->pos.x = 0;
        text->pos.y++;
        break;
    case KEY_UP:
        move_up(text);
        break;
    case KEY_LEFT:
        move_left(text);
        break;
    case KEY_DOWN:
        move_down(text);
        break;
    case KEY_RIGHT:
        move_right(text);
        break;
    case 127:// DELETE
        break;
    case KEY_BACKSPACE:
        delete_char(text);
        break;
    case 27: // ESCAPE
        quit = true;
        break;
    default:
        if (0 <= ch && ch <= 127)
        {
            insert_char(text, ch);
            text->pos.x++;
        }
        break;
    }

    clear();
}

int main(void)
{
    initscr();
    raw();
    curs_set(1);
    noecho();
    keypad(stdscr, true);

    Text text = init_text(NULL);

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        WINDOW *text_win = subwin(stdscr, max_y, max_x-2, 0, 1);
        waddstr(text_win, text.content);
        mvprintw(max_y-1, 0, "pos = %ld, %ld", text.pos.x, text.pos.y);
        mvprintw(max_y-2, 0, "size = %ld", text.size);
        mvprintw(max_y-3, 0, "line len = %ld", text.line[text.pos.y].len);

        move(text.pos.y, text.pos.x+1);
        wrefresh(text_win);
        key_events(&text);

        delwin(text_win);
    }

    free_text(&text);
    endwin();
    return 0;
}
