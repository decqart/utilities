#include <stdio.h>

int main(void)
{
    fputs("\033[3J\033[H\033[2J", stdout);
    fflush(stdout);
    return 0;
}
