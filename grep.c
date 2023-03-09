#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

char *read_file(const char *filepath, size_t *file_size)
{
    if (filepath == NULL) return NULL;
    FILE *file = fopen(filepath, "r");

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

void print_line(char *line)
{
    static char prev_line[256] = "";
    int size = 0;
    while (line[size] != '\n')
        size++;

    if (!strncmp(prev_line, line, size)) return;

    strncpy(prev_line, line, size);
    prev_line[size] = '\0';
    puts(prev_line);
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        puts("Usage: grep [pattern] [file]");
        exit(0);
    }

    char *file_name = NULL;
    char *pattern = NULL;

    bool patt_assigned = false;
    bool show_line_num = false;

    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-n"))
            show_line_num = true;
        if (argv[i][0] != '-' && !patt_assigned)
        {
            pattern = argv[i];
            patt_assigned = true;
            continue;
        }
        if (argv[i][0] != '-' && patt_assigned)
            file_name = argv[i];
    }

    size_t patlen = strlen(pattern);
    size_t file_size = 0;
    char *contents = read_file(file_name, &file_size);

    bool compare = false;

    //printf("file size = %ld\n", file_size);
    //printf("pattern = %s\n", pattern);
    size_t count = 0;
    while (contents[count])
    {
        if (compare)
        {
            compare = false;
            if (!strncmp(&contents[count], pattern, patlen))
            {
                //printf("found \"%s\" at %ld\n", pattern, count);
                int start_diff = 0;
                while (contents[count-start_diff] != '\n')
                    start_diff += 1;
                print_line(&contents[count-start_diff+1]);
            }
        }

        count += patlen;
        if (count > file_size) count = file_size;

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
    //printf("count = %ld\n", count);
    free(contents);
}
