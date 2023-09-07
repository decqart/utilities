#include <stdio.h>
#include <unistd.h>
#include <dirent.h>

int main(int argc, char **argv)
{
    if (argv[1] != NULL)
        chdir(argv[1]);

    DIR *dir = opendir(".");

    struct dirent *ent = readdir(dir);

    while (ent != NULL)
    {
        if (ent->d_name[0] != '.')
        {
            fputs(ent->d_name, stdout);
            fputs("  ", stdout);
        }

        ent = readdir(dir);
    }

    closedir(dir);

    putchar('\n');

    return 0;
}
