#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fts.h>

#define STRA_IMPLEMENTATION
#include <StringArray.h>

char *pattern = NULL;
size_t patlen = 0;

bool show_line = true;
bool show_count = false;
bool show_line_num = false;
bool show_file_name = false;
bool recursive_search = false;

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

void traverse_file_tree(StringArray *files)
{
    char *path[2] = { ".", NULL };

    FTS *tree = fts_open(path, 0, 0);
    if (tree == NULL) return;

    FTSENT *node = fts_read(tree);

    while (node != NULL)
    {
        if (node->fts_info & FTS_F)
        {
            char *file = strdup(node->fts_path);
            stra_append(files, &file[2]);
        }
        node = fts_read(tree);
    }

    fts_close(tree);
}

void print_line(char *line, size_t pos, const char *file_name)
{
    if (!show_line) return;
    static char *prev_line = "";
    static char *old_file_name = "";
    static size_t old_size = 0;
    size_t size = 0;
    while (line[size] != '\n')
        size++;

    if (!strcmp(old_file_name, file_name) &&
        !strncmp(prev_line, line, size))
        return;

    if (show_file_name)
        printf("%s:", file_name);

    if (show_line_num)
    {
        size_t nl_count = 1;
        while (pos--)
        {
            if (*(line-pos) == '\n')
                nl_count++;
        }
        printf("%ld:", nl_count);
    }

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

void parse_opts(const char *opts)
{
    while (*opts)
    {
        if (*opts == 'n')
        {
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

void search_file(FILE *file, const char *file_name)
{
    size_t len = 0;
    char *buffer = read_file(file, &len);
    if (buffer == NULL) return;

    bool compare = false;

    size_t found = 0;

    size_t i = 0;
    while (buffer[i])
    {
        if (compare)
        {
            compare = false;
            if (!strncmp(&buffer[i], pattern, patlen))
            {
                int start_diff = 0;
                while (buffer[i-start_diff] != '\n' && (i-start_diff+1 != 0))
                    start_diff++;

                print_line(&buffer[i-start_diff+1], i-start_diff+1, file_name);
                found++;
            }
        }

        i += patlen;
        if (i > len) break;

        for (int j = 0; pattern[j]; ++j)
        {
            if (buffer[i] == pattern[j])
            {
                i -= j;
                compare = true;
                break;
            }

            // checks for utf-8 continuation byte
            while ((pattern[j] & 0xc0) == 0x80)
                j++;
        }
    }
    if (show_count)
    {
        if (show_file_name)
            printf("%s:", file_name);
        printf("%ld\n", found);
    }

    free(buffer);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        puts("Usage: grep [OPTS] [PATTERN] [FILE]");
        return 0;
    }

    StringArray files = stra_init(50);

    bool patt_assigned = false;

    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-' && argv[i][1] != '\0')
            parse_opts(argv[i]);
        else if (patt_assigned)
            stra_append(&files, argv[i]);
        if (argv[i][0] != '-' && !patt_assigned)
        {
            pattern = argv[i];
            patt_assigned = true;
            continue;
        }
    }

    patlen = strlen(pattern);

    if (recursive_search && files.data[0] == NULL)
        traverse_file_tree(&files);
    else
    {
        show_file_name = files.pos > 1;
        recursive_search = false;
    }

    if (files.pos == 0)
        search_file(stdin, "");

    for (size_t i = 0; i < files.pos; ++i)
    {
        if (!strcmp(files.data[i], "-"))
        {
            search_file(stdin, "(standard input)");
            continue;
        }

        FILE *f = fopen(files.data[i], "r");
        if (f == NULL)
        {
            fprintf(stderr, "grep: '%s' is not a valid file or directory\n", files.data[i]);
            continue;
        }
        search_file(f, files.data[i]);
        fclose(f);
        if (recursive_search)
            free(files.data[i]-2);
    }

    stra_destroy(&files);
    return 0;
}
