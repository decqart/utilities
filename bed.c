#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <curses.h>

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

Pos max = { 0, 0 };
bool quit = false;

char *read_file(const char *filepath, size_t *file_size)
{
    if (filepath == NULL) return NULL;
    FILE *file = fopen(filepath, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    if (size < 0) goto error;
    *file_size = size;

    char *buffer = malloc(size+1);
    if (buffer == NULL) goto error;

    fseek(file, 0, SEEK_SET);

    fread(buffer, 1, size, file);

    buffer[size] = '\0';

    fclose(file);
    return buffer;
error:
    fclose(file);
    return NULL;
}

inline size_t get_newline_count(const char *str)
{
    size_t count = 0;
    while (*str++)
        if (*str == '\n')
            count++;
    return count;
}

Lines init_lines(size_t size, size_t used)
{
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

Text init_text_from_file(char *file_name)
{
    char *buffer = NULL;
    size_t filled_size = 0;
    size_t buffer_size = 0;
    Lines lines;

    if (file_name != NULL)
    {
        buffer = read_file(file_name, &filled_size);
        size_t nl_count = get_newline_count(buffer);

        lines = init_lines(nl_count*2, nl_count);

        size_t line_len = 0;
        size_t line_num = 0;
        for (int i = 0; i < filled_size; ++i)
        {
            line_len++;
            if (buffer[i] == '\n')
            {
                lines.idx[line_num].len = line_len;
                line_num++;
                line_len = 0;
            }
        }
        buffer_size = filled_size+1;
    }
    else
    {
        buffer = malloc(128 * sizeof(char));
        buffer[0] = '\0';
        buffer_size = 128;
        lines = init_lines(32, 0);
        for (int i = 0; i < 32; ++i)
            lines.idx[i].len = 0;
    }

    return (Text) { buffer, filled_size, buffer_size, (Pos) { 0, 0 }, lines };
}

size_t get_actual_pos(Text text)
{
    size_t pos = 0;
    for (int i = 0; i < text.pos.y; ++i)
        pos += text.lines.idx[i].len;

    return pos+text.pos.x;
}

void move_up(Text *text)
{
    if (text->pos.y == 0) return;

    text->pos.y--;
    if (text->lines.idx[text->pos.y].len-1 < text->pos.x)
        text->pos.x = text->lines.idx[text->pos.y].len-1;
}

void move_down(Text *text)
{
    if (text->lines.idx[text->pos.y+1].len != 0)
    {
        text->pos.y++;
        if (text->lines.idx[text->pos.y].len-1 <= text->pos.x)
        {
            text->pos.x = text->lines.idx[text->pos.y].len-1;
            if (text->pos.y == text->lines.used)
                text->pos.x++;
        }
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
    size_t pos = get_actual_pos(*text);
    if (text->content[pos] == '\n')
    {
        text->pos.y++;
        text->pos.x = 0;
    }
    else if (text->lines.idx[text->pos.y].len != text->pos.x)
        text->pos.x++;
}

//FIXIT
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

    size_t pos = get_actual_pos(*buf);
    for (size_t i = buf->filled-1; pos < i; --i)
        buf->content[i] = buf->content[i-1];

    buf->content[pos] = ch;
}

//FIXIT
void delete_char(Text *buf)
{
    Line *line = buf->lines.idx;

    size_t pos = get_actual_pos(*buf);
    if (buf->content[pos] == '\0') return;

    if (buf->content[pos] == '\n')
    {
        line[buf->pos.y].len += line[buf->pos.y+1].len;

        for (int i = buf->pos.y+1; i < buf->lines.count; ++i)
            line[i].len = line[i+1].len;
        buf->lines.used--;
    }
    buf->lines.idx[buf->pos.y].len--;

    for (size_t i = pos; i < buf->filled; ++i)
        buf->content[i] = buf->content[i+1];

    buf->filled--;
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

#define ctrl(x) ((x) & 0x1f)

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
        if (text->pos.x == 0 && text->pos.y == 0) break;
        move_left(text);
        delete_char(text);
        break;
    case ctrl('x'):
        quit = true;
        break;
    default:
        if (31 < ch && ch < 127)
        {
            insert_char(text, ch);
            text->pos.x++;
        }
        break;
    }

    clear();
}

void print_text(Text text)
{
    size_t line_num = 0;
    size_t line_idx = 0;
    if (text.pos.y > max.y-1)
        line_num = max.y-1 - text.pos.y;
    while (*text.content)
    {
        mvaddch(line_num, line_idx, *text.content);
        line_idx++;
        if (*text.content == '\n')
        {
            line_num++;
            line_idx = 0;
        }
        text.content++;
    }
}

void update_cursor(Pos pos)
{
    move(pos.y, pos.x);
}

int main(int argc, char **argv)
{
    initscr();
    raw();
    curs_set(1);
    noecho();
    keypad(stdscr, true);

    Text text = init_text_from_file(argv[1]);

    while (!quit)
    {
        getmaxyx(stdscr, max.y, max.x);
        print_text(text);
        mvprintw(max.y-1, 0, "char = '%c'", text.content[get_actual_pos(text)]);
        mvprintw(max.y-2, 0, "size = %ld", text.size);
        mvprintw(max.y-3, 0, "line len = %ld", text.lines.idx[text.pos.y].len);
        mvprintw(max.y-1, 12, "line size = %ld", text.lines.used);

        update_cursor(text.pos);
        key_events(&text);
    }

    free_text(&text);
    endwin();
    return 0;
}
