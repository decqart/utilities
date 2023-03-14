#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>

int max_y = 0, max_x = 0;
bool quit = false;

typedef struct {
    size_t x;
    size_t y;
} Pos;

typedef struct {
    size_t len;
} Line;

typedef struct {
    char *content;
    size_t filled;
    size_t size;
    Pos pos;
    Line *line;
} Text;

size_t get_newline_count(const char *str)
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
    if (cont != NULL)
    {
        size_t nl_count = get_newline_count(cont)+1;
        size_t cont_len = strlen(cont);
        buffer = malloc(cont_len * 2 * sizeof(char));
        memcpy(buffer, cont, cont_len+1);
        buffer_size = cont_len;

        line = malloc(nl_count * 2 * sizeof(Line));

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
        for (int i = 0; i < 20; ++i)
            line[i].len = 0;
    }

    return (Text) { buffer, filled_size, buffer_size, (Pos) { 0, 0 }, line };
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
        text->pos.y--;
}

void move_down(Text *text)
{

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
    buf->line[buf->pos.y].len++;
    for (size_t i = buf->filled-1; pos < i; --i)
        buf->content[i+1] = buf->content[i];

    buf->content[pos] = ch;
}

void delete_char(Text *buf)
{
    if (buf->pos.x == 0 && buf->pos.y == 0) return;
    move_left(buf);
    buf->filled--;
    size_t pos = get_actual_pos(buf);
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
    int height = 0, width = 0;
    initscr();
    raw();
    curs_set(1);
    noecho();
    keypad(stdscr, true);

    Text text = init_text(NULL);

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        mvprintw(0, 0, text.content);
        mvprintw(max_y-1, 0, "pos = %ld, %ld", text.pos.x, text.pos.y);
        mvprintw(max_y-2, 0, "size = %ld", text.size);

        move(text.pos.y, text.pos.x);
        key_events(&text);

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
