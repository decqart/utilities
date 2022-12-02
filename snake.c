#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

int max_x, max_y;

int score = 0;
int body_size = 1;

int quit = 0;

typedef struct {
    int x;
    int y;
} vec;

int apple_x, apple_y;

void apple()
{
    srand(time(NULL));
    
    apple_x = rand() % (max_x-1);
    apple_y = rand() % (max_y-1);
    mvprintw(apple_y, apple_x, "A");
}

int snake_y = 2;
int snake_x = 2;
vec snke_dir = { .x=1, .y=0 };

void snake()
{
    switch(getch())
    {
    case 'w':
        snke_dir.x = 0;
        if (snke_dir.y != 1)
            snke_dir.y = -1;
        break;
    case 'a':
        if (snke_dir.x != 1)
            snke_dir.x = -1;
        snke_dir.y = 0;
        break;
    case 's':
        snke_dir.x = 0;
        if (snke_dir.y != -1)
            snke_dir.y = 1;
        break;
    case 'd':
        if (snke_dir.x != -1)
            snke_dir.x = 1;
        snke_dir.y = 0;
        break;
    case 'q':
        quit = 1;
    default:
        break;
    }
    snake_x += snke_dir.x;
    snake_y += snke_dir.y;
    mvprintw(snake_y-snke_dir.y, snake_x-snke_dir.x, " ");
    mvprintw(snake_y, snake_x, "O");

    if (snake_y == apple_y && snake_x == apple_x)
    {
        apple();
        score++;
        body_size++;
    }
    
    if (snke_dir.y == -1 || snke_dir.y == 1)
        usleep(80000);

    if (snake_y < 0 || snake_x < 0 ||
        snake_x > max_x || snake_y > max_y)
    {
        endwin();
        printf("Score: %d\n", score);
        printf("GAME OVER\n");
        exit(0);
    }
}

void render_score()
{
    char num[12];
    sprintf(num, "Score: %d", score);
    mvprintw(0, 0, num);
}

int main()
{   
    initscr();
    raw();
    curs_set(0);
    noecho();
    nodelay(stdscr, 1);
    getmaxyx(stdscr, max_y, max_x);
    /* box(stdscr, ACS_VLINE, ACS_HLINE); */

    apple(); /* first apple to start it off */
    while (!quit)
    {
        render_score();
        snake();
        usleep(100000);
    }
    endwin();
    return 0;
}

