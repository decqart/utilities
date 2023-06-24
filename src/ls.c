#include <stdio.h>
#include <dirent.h>

int main(int argc, char **argv)
{
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
