#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curses.h>

int max_y = 0, max_x = 0;
int submax_y, submax_x;
int mouse_y, mouse_x;
long num = 0, numb = 0, d_answer = 0;
int ch;
int quit = 0;
char answer[100] = {0};

void draw_keypad(void)
{
    WINDOW *subwindow = subwin(stdscr, max_y-4, max_x-4, 3, 2);
    box(subwindow, ACS_VLINE, ACS_HLINE);
    getmaxyx(subwindow, submax_y, submax_x);
    mvwhline(subwindow, submax_y/5,   1, ACS_HLINE, max_x-6);
    mvwhline(subwindow, submax_y*3/7, 1, ACS_HLINE, max_x-6);
    mvwhline(subwindow, submax_y*3/5, 1, ACS_HLINE, max_x-6);
    mvwhline(subwindow, submax_y*3/4, 1, ACS_HLINE, max_x-6);
    mvwvline(subwindow, 1, submax_x/4,   ACS_VLINE, submax_y-2);
    mvwvline(subwindow, 1, submax_x/2,   ACS_VLINE, submax_y-2);
    mvwvline(subwindow, 1, submax_x*3/4, ACS_VLINE, submax_y-2);
}

void calculate(void)
{
    int symbol = strcspn(answer, "+-*/");
    char numstr[20];
    char *numbstr;
    strncpy(numstr, answer, symbol);
    num = atoi(numstr);
    
    switch (answer[symbol])
    {
    case '+':
        numbstr = strchr(answer, '+');
        numbstr[0] = ' ';
        numb = atol(numbstr);
        d_answer = num + numb;
        break;
    case '-':
        numbstr = strchr(answer, '-');
        numbstr[0] = ' ';
        numb = atol(numbstr);
        d_answer = num - numb;
        break;
    case '*':
        numbstr = strchr(answer, '*');
        numbstr[0] = ' ';
        numb = atol(numbstr);
        d_answer = num * numb;
        break;
    case '/':
        numbstr = strchr(answer, '/');
        numbstr[0] = ' ';
        numb = atol(numbstr);
        d_answer = num / numb;
        break;
    default:
        break;
    }
    sprintf(answer, "%ld", d_answer);
    clear();
}

int mouse_input_box(int x1, int y1, int x2, int y2)
{
    if (x1 < mouse_x && mouse_x < x2 &&
        y1 < mouse_y && mouse_y < y2)
        return 1;
    return 0;
}

void key_events(void)
{
    ch = getch();

    if (mouse_input_box(2, 3, 11, 6))
    {
        ch = 'c';
    }

    /* TODO: add button locations */
    
    switch (ch)
    {
    case '1':
        strcat(answer, "1");
        break;
    case '2':
        strcat(answer, "2");
        break;
    case '3':
        strcat(answer, "3");
        break;
    case '4':
        strcat(answer, "4");
        break;
    case '5':
        strcat(answer, "5");
        break;
    case '6':
        strcat(answer, "6");
        break;
    case '7':
        strcat(answer, "7");
        break;
    case '8':
        strcat(answer, "8");
        break;
    case '9':
        strcat(answer, "9");
        break;
    case '0':
        strcat(answer, "0");
        break;
    case '+':
        strcat(answer, "+");
        break;
    case '-':
        strcat(answer, "-");
        break;
    case '/':
        if (answer[strlen(answer)-1] != '/' &&
            answer[strlen(answer)-1] != '*')
            strcat(answer, "/");
        break;
    case '*':
        if (answer[strlen(answer)-1] != '/' &&
            answer[strlen(answer)-1] != '*')
            strcat(answer, "*");
        break;
    case KEY_ENTER:
    case '\n':
        calculate();
        break;
    case KEY_BACKSPACE:
        answer[strlen(answer)-1] = 0;
        clear();
        break;
    case 'c':
        /* CLEAR */
        answer[0] = '\0';
        break;
    case 'q':
        quit = 1;
        break;
    default:
        break;
    }
    MEVENT event;
    if (getmouse(&event) == OK)
    {
        clear();
        mvprintw(4, 2, "Mouse at row=%d, column=%d\nbstate=%d", event.y, event.x, event.bstate);
        mouse_y = event.y;
        mouse_x = event.x;
        if (event.bstate == 2)
        {
            mouse_y = -1;
            mouse_x = -1;
        }
    }
}

int main(void)
{
    int height = max_y, width = max_x;
    initscr();
    raw();
    curs_set(0);
    noecho();
    mousemask(BUTTON1_PRESSED | BUTTON1_RELEASED, NULL);
    keypad(stdscr, 1);

    while (!quit)
    {
        getmaxyx(stdscr, max_y, max_x);
        box(stdscr, ACS_VLINE, ACS_HLINE);
        mvprintw(1, max_x-strlen(answer)-1, answer);
        mvhline(2, 1, ACS_HLINE, max_x-2);
        draw_keypad();

        key_events();

        if (max_x != width || max_y != height)
        {
            width = max_x;
            height = max_y;
            clear();
        }
    }

    endwin();

    return 0;
}

