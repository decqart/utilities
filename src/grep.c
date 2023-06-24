#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fts.h>

#define STRA_IMPLEMENTATION
#include <StrArray.h>

char *read_file(const char *file_path, size_t *file_size)
{
    FILE *file = fopen(file_path, "r");
    if (file == NULL) return NULL;

    fseek(file, 0, SEEK_END);

    long size = ftell(file);
    if (size < 0) goto error;
    if (file_size != NULL)
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

void traverse_file_tree(StrArray *files)
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

char *pattern = NULL;
size_t patlen = 0;

bool show_line = true;
bool show_count = false;
bool show_line_num = false;
bool show_file_name = false;
bool recursive_search = false;

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

void search_file(const char *file_name)
{
    size_t cont_len = 0;
    char *contents = read_file(file_name, &cont_len);
    if (contents == NULL) return;

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
                    start_diff++;
                }
                print_line(&contents[count-start_diff+1], count-start_diff+1, file_name);
                found++;
            }
        }

        count += patlen;
        if (count > cont_len) break;

        for (int i = 0; pattern[i]; ++i)
        {
            if (contents[count] == pattern[i])
            {
                count -= i;
                compare = true;
                break;
            }

            // checks for utf-8 continuation byte
            while ((pattern[i] & 0xc0) == 0x80)
                i++;
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
    //TODO: read stdin when needed
    if (argc < 3)
    {
        puts("Usage: grep [OPTS] [PATTERN] [FILE]");
        return 0;
    }

    StrArray files = stra_init(50);

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

    if (recursive_search && files.value[0] == NULL)
        traverse_file_tree(&files);
    else
    {
        show_file_name = files.value[1] != NULL;
        recursive_search = false;
    }

    for (int i = 0; files.value[i] != NULL; ++i)
    {
        search_file(files.value[i]);
        if (recursive_search)
            free(files.value[i]-2);
    }

    stra_destroy(&files);
    return 0;
}
