#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    char *str = "y";
    if (argc > 1)
    {
        size_t len = 0;
        for (int i = 1; i < argc; ++i)
            len += strlen(argv[i])+1;

        str = malloc(len);
        str[0] = '\0';
        for (int i = 1; i < argc; ++i)
        {
            strcat(str, argv[i]);
            strcat(str, " ");
        }
        str[len-1] = '\0';
    }
    while (puts(str) != EOF);
    return 0;
}
