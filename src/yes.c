#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
    char *string = "y";
    if (argc > 1)
    {
        size_t len = 0;
        for (int i = 1; i < argc; ++i)
        {
            len += strlen(argv[i])+1;
        }
        string = malloc(len*sizeof(char));
        string[0] = '\0';
        for (int i = 1; i < argc; ++i)
        {
            strcat(string, argv[i]);
            strcat(string, " ");
        }
        string[len-1] = '\0';
    }
    while (puts(string) != EOF);
    return 0;
}
