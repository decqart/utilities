#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

#include <DyArray.h>

typedef DyArray(char *) StringArray;

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

    bool whitespace = true;

    size_t i = 0;
    while (buffer[i])
    {
        if (isspace(buffer[i]) && !whitespace)
            word_count++;
        whitespace = isspace(buffer[i]);

        if (buffer[i] == '\n')
            line_count++;

        if ((buffer[i] & 0xc0) != 0x80)
            char_count++;
        i++;
    }

    if (show_all || (show_lines && (show_words || show_chars || show_bytes)))
        putchar(' ');
    if (show_all || show_lines)
        printf("%ld ", line_count);

    if (show_all || show_words)
        printf("%ld ", word_count);

    if (show_chars)
        printf("%ld ", char_count);

    if (show_all || show_bytes)
        printf("%ld ", i);

    puts(file_name);

    total_line += line_count;
    total_word += word_count;
    total_byte += i;
    total_char += char_count;

    free(buffer);
}

int main(int argc, char **argv)
{
    StringArray files = da_init(char *, 50);

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
            parse_opts(argv[i]);
        else
            da_append(char *, files, argv[i]);
    }

    if (files.size == 0)
        process_file(stdin, "");

    for (size_t i = 0; i < files.size; ++i)
    {
        if (!strcmp(files.data[i], "-"))
        {
            process_file(stdin, files.data[i]);
            continue;
        }

        FILE *f = fopen(files.data[i], "r");
        if (f == NULL)
        {
            fprintf(stderr, "wc: '%s' is not a valid file or directory\n", files.data[i]);
            continue;
        }
        process_file(f, files.data[i]);
        fclose(f);
    }

    if (files.size > 1)
    {
        if (show_all || (show_lines && (show_words || show_chars || show_bytes)))
            putchar(' ');
        if (show_all || show_lines)
            printf("%ld ", total_line);

        if (show_all || show_words)
            printf("%ld ", total_word);

        if (show_chars)
            printf("%ld ", total_char);

        if (show_all || show_bytes)
            printf("%ld ", total_byte);

        puts("total");
    }

    da_destroy(files);
    return 0;
}
