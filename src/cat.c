#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_IMPLEMENTATION
#include <File.h>

char *read_input(void)
{
    char *buffer = malloc(256);
    if (buffer == NULL) return NULL;
    size_t size = 256;
    size_t pos = 0;

    while (!feof(stdin))
    {
        int ch = getchar();
        if (ch == '\n') break;
        buffer[pos++] = ch;
        if (pos == size)
        {
            size <<= 1;
            buffer = realloc(buffer, size);
            if (buffer == NULL) return NULL;
        }
    }
    buffer[pos] = '\0';
    return buffer;
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-"))
        {
            while (1)
            {
                char *buffer = read_input();
                if (buffer != NULL)
                {
                    puts(buffer);
                    free(buffer);
                }
            }
        }

        char *buffer = read_file(argv[i], NULL);
        if (buffer != NULL)
        {
            fputs(buffer, stdout);
            free(buffer);
        }
    }

    return 0;
}
