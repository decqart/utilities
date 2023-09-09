#include <stdio.h>
#include <stdbool.h>

#define STRA_IMPLEMENTATION
#include <StringArray.h>

size_t total_line = 0;
size_t total_word = 0;
size_t total_char = 0;
size_t total_byte = 0;

bool show_all = true;
bool show_lines = false;
bool show_words = false;
bool show_chars = false;
bool show_bytes = false;

char *read_file(FILE *file, size_t *file_size)
{
    size_t size = 1024, pos = 0;
    char *data = malloc(1024);

    while (!feof(file))
    {
        if (pos == size-1)
        {
            size <<= 1;
            data = realloc(data, size);
        }

        pos += fread(&data[pos], 1, size-1-pos, file);
    }

    if (file_size != NULL)
        *file_size = pos;
    data[pos] = '\0';

    return data;
}

void parse_opts(const char *opts)
{
    while (*opts)
    {
        if (*opts == 'l')
        {
            show_lines = true;
            show_all = false;
        }
        else if (*opts == 'w')
        {
            show_words = true;
            show_all = false;
        }
        else if (*opts == 'c')
        {
            show_bytes = true;
            show_all = false;
        }
        else if (*opts == 'm')
        {
            show_chars = true;
            show_all = false;
        }
        opts++;
    }
}

void process_file(FILE *file, const char *file_name)
{
    char *buffer = read_file(file, NULL);
    if (buffer == NULL) return;

    size_t line_count = 0;
    size_t word_count = 0;
    size_t char_count = 0;

    size_t i = 0;
    while (buffer[i])
    {
        if (buffer[i] == '\n')
            line_count++;

        if ((buffer[i] & 0xc0) != 0x80)
            char_count++;
        i++;
    }

    if (show_all) putchar(' ');
    if (show_all || show_lines)
        printf("%ld ", line_count);

    if (show_all || show_words)
        printf("%ld ", word_count);

    if (show_chars)
        printf("%ld ", char_count);

    if (show_all || show_bytes)
        printf("%ld ", i);

    puts(file_name);

    free(buffer);
}

int main(int argc, char **argv)
{
    StringArray files = stra_init(50);

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            parse_opts(argv[i]);
        else
            stra_append(&files, argv[i]);
    }

    if (files.pos == 0)
        process_file(stdin, "");

    for (size_t i = 0; i < files.pos; ++i)
    {
        FILE *f = fopen(files.data[i], "r");
        process_file(f, files.data[i]);
        fclose(f);
    }

    stra_destroy(&files);
    return 0;
}
