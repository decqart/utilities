#include <stdio.h>
#include <stdbool.h>

#include <sys/utsname.h>

#ifdef __gnu_linux__
#define OS_NAME "GNU/Linux"
#elif defined(__linux__)
#define OS_NAME "Linux"
#elif defined(__FreeBSD__)
#define OS_NAME "FreeBSD"
#elif defined(__OpenBSD__)
#define OS_NAME "OpenBSD"
#else
#define OS_NAME "unknown"
#endif

bool show_all = false;
bool show_sysname = true;
bool show_nodename = false;
bool show_release = false;
bool show_version = false;
bool show_machine = false;
bool show_processor = false;
bool show_hardware = false;
bool show_os = false;

void parse_opts(const char *opts)
{
    show_sysname = false;
    while (*opts)
    {
        if (*opts == 'a')
            show_all = true;
        else if (*opts == 's')
            show_sysname = true;
        else if (*opts == 'n')
            show_nodename = true;
        else if (*opts == 'r')
            show_release = true;
        else if (*opts == 'v')
            show_version = true;
        else if (*opts == 'm')
            show_machine = true;
        else if (*opts == 'p')
            show_processor = true;
        else if (*opts == 'i')
            show_hardware = true;
        else if (*opts == 'o')
            show_os = true;
        opts++;
    }
}

int main(int argc, char **argv)
{
    for (int i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
            parse_opts(argv[i]);
    }

    struct utsname uts;
    uname(&uts);

    int pad = 0;

    if (show_all || show_sysname)
    {
        fputs(uts.sysname, stdout);
        pad++;
    }

    if (show_all || show_nodename)
    {
        if (pad) putchar(' ');
        fputs(uts.nodename, stdout);
        pad++;
    }

    if (show_all || show_release)
    {
        if (pad) putchar(' ');
        fputs(uts.release, stdout);
        pad++;
    }

    if (show_all || show_version)
    {
        if (pad) putchar(' ');
        fputs(uts.version, stdout);
        pad++;
    }

    if (show_all || show_machine)
    {
        if (pad) putchar(' ');
        fputs(uts.machine, stdout);
        pad++;
    }

    if (!show_all && show_processor)
    {
        if (pad) putchar(' ');
        fputs("unknown", stdout);
        pad++;
    }

    if (!show_all && show_hardware)
    {
        if (pad) putchar(' ');
        fputs("unknown", stdout);
        pad++;
    }

    if (show_all || show_os)
    {
        if (pad) putchar(' ');
        fputs(OS_NAME, stdout);
    }

    putchar('\n');

    return 0;
}
