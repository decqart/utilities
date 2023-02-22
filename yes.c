#include <stdio.h>
#include <string.h>

int main(int argc, char **argv)
{
    char string[512] = "y";
    if (argc > 0)
        string[0] = '\0';
    for (int i = 1; i < argc; ++i)
    {
        strcat(string, argv[i]);
        strcat(string, " ");
    }
    while (1)
        puts(string);
}
