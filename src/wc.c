#include <stdio.h>
#include <stdbool.h>

#define STRA_IMPLEMENTATION
#include <StringArray.h>

size_t line_count = 0;
size_t word_count = 0;
size_t char_count = 0;

bool show_all = true;
bool show_lines = false;
bool show_words = false;
bool show_chars = false;

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
            show_chars = true;
            show_all = false;
        }
        opts++;
    }
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

    stra_destroy(&files);
    return 0;
}
