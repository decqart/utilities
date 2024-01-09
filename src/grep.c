#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <fts.h>

#include <StringArray.h>

char *pattern = NULL;
size_t patlen = 0;

bool show_line = true;
bool show_count = false;
bool show_line_num = false;
bool show_file_name = false;
bool recursive_search = false;

void *read_file(FILE *file, size_t *file_size)
{
    size_t alloced = 1024, size = 0;
    char *data = malloc(1024);

    while (!feof(file))
    {
        if (size == alloced)
        {
            alloced <<= 1;
            data = realloc(data, alloced);
        }

        size += fread(&data[size], 1, alloced-size, file);
    }

    *file_size = size;

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

void print_line(const char *line, size_t pos, const char *file_name)
{
    if (!show_line) return;

    size_t size = 0;
    while (line[size] != '\n')
        size++;

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
        printf("%lu:", nl_count);
    }

    fwrite(line, 1, size, stdout);
    putchar('\n');
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

    size_t found = 0;

    size_t i = 0;
    while (buffer[i] && i < len)
    {
        for (size_t j = 0; pattern[j]; ++j)
        {
            if (buffer[i] == pattern[j])
            {
                i -= j;

                if (!strncmp(&buffer[i], pattern, patlen))
                {
                    size_t diff = 0;
                    while (buffer[i-diff] != '\n' && (i-diff+1 != 0))
                        diff++;

                    size_t start = i-diff+1;
                    print_line(&buffer[start], start, file_name);
                    found++;

                    while (buffer[i] && buffer[i] != '\n')
                        i++;
                }

                break;
            }

            // checks for utf-8 continuation byte
            //while ((pattern[j] & 0xc0) == 0x80)
            //    j++;
        }

        i += patlen;
    }

    if (show_count)
    {
        if (show_file_name)
            printf("%s:", file_name);
        printf("%lu\n", found);
    }

    free(buffer);
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        fprintf(stderr, "Usage: grep [OPTS] [PATTERN] [FILE]\n");
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
        show_file_name = files.size > 1;
        recursive_search = false;
    }

    if (files.size == 0)
        search_file(stdin, "");

    for (size_t i = 0; i < files.size; ++i)
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
