#include <stdio.h>
#include <unistd.h>

int main(void)
{
    puts(getlogin());
    return 0;
}
