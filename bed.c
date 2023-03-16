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
    Line *idx;
    size_t count;
    size_t used;
} Lines;

typedef struct {
    char *content;
    size_t filled;
    size_t size;
    Pos pos;
    Lines lines;
} Text;

inline size_t get_newline_count(const char *str)
{
    size_t count = 0;
    while (*str++)
        if (*str == '\n')
            count++;
    return count;
}

Lines init_lines(size_t line_count)
{
    size_t size = line_count * 2;
    size_t used = line_count;
    Line *line = malloc(size * sizeof(Line));
    return (Lines) { line, size, used };
}

void update_lines(Lines *line)
{
    if (line->count == line->used)
    {
        line->count <<= 1;
        line->idx = realloc(line->idx, line->count * sizeof(Line));
    }
}

Text init_text(char *cont)
{
    char *buffer = NULL;
    size_t filled_size = 0;
    size_t buffer_size = 0;
    Lines lines;

    if (cont != NULL)
    {
        size_t nl_count = get_newline_count(cont)+1;
        size_t cont_len = strlen(cont);
        buffer = malloc(cont_len * 2 * sizeof(char));
        memcpy(buffer, cont, cont_len+1);
        buffer_size = cont_len;

        lines = init_lines(nl_count);

        size_t line_len = 0;
        size_t line_num = 0;
        for (int i = 0; i < buffer_size; ++i)
        {
            if (buffer[i] == '\n')
            {
                lines.idx[line_num].len = line_len;
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
        lines = init_lines(32);
        for (int i = 0; i < 20; ++i)
            lines.idx[i].len = 0;
    }

    return (Text) { buffer, filled_size, buffer_size, (Pos) { 0, 0 }, lines };
}

size_t get_actual_pos(Text *text)
{
    size_t pos = 0;
    for (int i = 0; i < text->pos.y; ++i)
        pos += text->lines.idx[i].len;

    return pos+text->pos.x;
}

void move_up(Text *text)
{
    if (text->pos.y != 0)
    {
        text->pos.y--;
        if (text->lines.idx[text->pos.y].len-1 < text->pos.x)
            text->pos.x = text->lines.idx[text->pos.y].len-1;
    }
}

void move_down(Text *text)
{
    //FIXIT: replace cursor based on if line has '\n'
    if (text->lines.idx[text->pos.y+1].len != 0)
    {
        text->pos.y++;
        if (text->lines.idx[text->pos.y].len-1 <= text->pos.x)
            text->pos.x = text->lines.idx[text->pos.y].len-1;
    }
}

void move_left(Text *text)
{
    if (text->pos.x == 0 && text->pos.y != 0)
    {
        text->pos.y--;
        text->pos.x = text->lines.idx[text->pos.y].len;
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
    else if (text->lines.idx[text->pos.y].len != text->pos.x)
        text->pos.x++;
}

void insert_char(Text *buf, char ch)
{
    Line *line = buf->lines.idx;

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
        for (int i = buf->lines.count; buf->pos.y+1 < i; --i)
            line[i].len = line[i-1].len;

        line[buf->pos.y+1].len = line[buf->pos.y].len - buf->pos.x;
        line[buf->pos.y].len = buf->pos.x;
        buf->lines.used++;
        update_lines(&buf->lines);
    }
    line[buf->pos.y].len++;
    for (size_t i = buf->filled; pos < i; --i)
        buf->content[i] = buf->content[i-1];

    buf->content[pos] = ch;
}

void delete_char(Text *buf)
{
    Line *line = buf->lines.idx;

    size_t pos = get_actual_pos(buf);
    if (buf->content[pos] == '\0') return;
    buf->filled--;

    if (buf->content[pos] == '\n')
    {
        line[buf->pos.y].len += line[buf->pos.y+1].len-1;

        for (int i = buf->pos.y+1; i < buf->lines.count; ++i)
            line[i].len = line[i+1].len;
        buf->lines.used--;
    }
    else
        buf->lines.idx[buf->pos.y].len--;

    for (size_t i = pos; i < buf->filled; ++i)
        buf->content[i] = buf->content[i+1];

    buf->content[buf->filled] = '\0';
}

void free_text(Text *buf)
{
    free(buf->content);
    free(buf->lines.idx);
    buf->content = NULL;
    buf->lines.idx = NULL;
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
    case KEY_DC:// DELETE
        delete_char(text);
        break;
    case KEY_BACKSPACE:
        if (text->pos.x == 0 && text->pos.y == 0) return;
        move_left(text);
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
        mvprintw(max_y-3, 0, "line len = %ld", text.lines.idx[text.pos.y].len);
        mvprintw(max_y-1, 12, "line size = %ld", text.lines.count);

        move(text.pos.y, text.pos.x+1);
        key_events(&text);

        delwin(text_win);
    }

    free_text(&text);
    endwin();
    return 0;
}
