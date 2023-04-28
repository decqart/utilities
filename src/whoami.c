#include <stdio.h>
#include <unistd.h>

int main(void)
{
    char *name = getlogin();
    if (name != NULL)
        puts(name);
    return 0;
}
