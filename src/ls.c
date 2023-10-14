#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    if (argv[1] != NULL)
    {
        if (chdir(argv[1]) == -1)
        {
            printf("ls: cant access '%s'\n", argv[1]);
            return 1;
        }
    }

    int piped = !isatty(STDOUT_FILENO);

    DIR *dir = opendir(".");

    struct dirent *ent = readdir(dir);

    while (ent != NULL)
    {
        if (ent->d_name[0] != '.')
        {
            fputs(ent->d_name, stdout);

            if (piped)
                fputc('\n', stdout);
            else
                fputs("  ", stdout);
        }

        ent = readdir(dir);
    }

    closedir(dir);

    if (!piped)
        putchar('\n');

    return 0;
}
