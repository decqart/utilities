#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define STRA_IMPLEMENTATION
#include "StrArray.h"

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

typedef struct {
    size_t *value;
    size_t size;
} UIntArray;

char *pattern = NULL;
size_t patlen = 0;

UIntArray nlines = { NULL, 0 };
bool show_line = true;
bool show_count = false;
bool show_line_num = false;
bool show_file_name = false;
bool recursive_search = false;

void get_newlines(const char *buffer)
{
    size_t idx = 0;
    size_t n_count = 1;
    nlines.value[0] = 0;
    while (buffer[idx])
    {
        if (buffer[idx] == '\n')
        {
            nlines.value[n_count] = idx;
            n_count++;
            if (n_count == nlines.size)
            {
                nlines.size <<= 1;
                nlines.value = realloc(nlines.value, nlines.size*sizeof(size_t *));
            }
        }
        idx++;
    }
}

size_t get_line_pos(size_t pos)
{
    for (int i = 0; i < nlines.size-1; ++i)
    {
        if (nlines.value[i] < pos && pos < nlines.value[i+1])
            return i+1;
    }
    return nlines.size;
}

void print_line(char *line, size_t pos, char *file_name)
{
    if (!show_line) return;
    static char *prev_line = "";
    static size_t old_size = 0;
    size_t size = 0;
    while (line[size] != '\n')
        size++;

    if (!strncmp(prev_line, line, size)) return;

    if (show_file_name)
        printf("%s:", file_name);

    if (show_line_num)
        printf("%ld:", get_line_pos(pos));

    if (old_size < size)
    {
        if (prev_line[0] != '\0')
            free(prev_line);
        prev_line = malloc(size+1);
    }
    old_size = size;
    strncpy(prev_line, line, size);
    prev_line[size] = '\0';
    puts(prev_line);
}

void parse_opts(char *opts)
{
    while (*opts)
    {
        if (*opts == 'n')
        {
            nlines.value = malloc(256*sizeof(size_t *));
            nlines.size = 256;
            show_line_num = true;
        }
        else if (*opts == 'r')
        {
            recursive_search = true;
            show_file_name = true;
        }
        else if (*opts == 'c')
        {
            show_line = false;
            show_count = true;
        }
        opts++;
    }
}

void search_file(char *file_name)
{
    size_t file_size = 0;
    char *contents = read_file(file_name, &file_size);
    if (contents == NULL) return;

    if (show_line_num)
        get_newlines(contents);

    bool compare = false;

    size_t found = 0;

    size_t count = 0;
    while (contents[count])
    {
        if (compare)
        {
            compare = false;
            if (!strncmp(&contents[count], pattern, patlen))
            {
                int start_diff = 0;
                while (contents[count-start_diff] != '\n' &&
                       (count-start_diff+1 != 0))
                {
                    start_diff += 1;
                }
                print_line(&contents[count-start_diff+1], count, file_name);
                found++;
            }
        }

        count += patlen;
        if (count > file_size) break;

        for (int i = 0; pattern[i]; ++i)
        {
            if (contents[count] == pattern[i])
            {
                count -= i;
                compare = true;
                break;
            }
        }
    }
    if (show_count)
    {
        if (show_file_name)
            printf("%s:", file_name);
        printf("%ld\n", found);
    }

    free(contents);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        puts("Usage: grep [OPTS] [PATTERN] [FILE]");
        exit(0);
    }

    StrArray files = stra_init();

    bool patt_assigned = false;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            parse_opts(argv[i]);
        if (argv[i][0] != '-' && !patt_assigned)
        {
            pattern = argv[i];
            patt_assigned = true;
            continue;
        }
        if (argv[i][0] != '-' && patt_assigned)
            stra_append(&files, argv[i]);
    }

    patlen = strlen(pattern);

    show_file_name = files.value[1] != NULL;

    for (int i = 0; files.value[i] != NULL; ++i)
        search_file(files.value[i]);

    stra_destroy(&files);
    free(nlines.value);
    return 0;
}
