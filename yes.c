#include <stdio.h>
#include <string.h>

#define STR_SIZE 1024

int main(int argc, char **argv)
{
    char string[STR_SIZE] = "y";
    if (argc > 1)
        string[0] = '\0';
    size_t len = 0;
    for (int i = 1; i < argc; ++i)
    {
        len += strlen(argv[i]);
        if (len > STR_SIZE)
        {
            puts("ERROR: string input too large");
            return 1;
        }
        strcat(string, argv[i]);
        strcat(string, " ");
    }
    while (1)
        puts(string);
}
