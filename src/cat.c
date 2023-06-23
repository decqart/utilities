#include <stdio.h>
#include <string.h>

void read_and_write(FILE *file)
{
    char buffer[256];

    while (fgets(buffer, sizeof(buffer), file) != NULL)
    {
        fputs(buffer, stdout);
        buffer[0] = '\0';
    }
}

int main(int argc, char **argv)
{
    if (argc == 1)
        read_and_write(stdin);

    for (int i = 1; i < argc; ++i)
    {
        if (!strcmp(argv[i], "-"))
        {
            read_and_write(stdin);
            break;
        }

        FILE *f = fopen(argv[i], "r");
        read_and_write(f);
    }

    return 0;
}
