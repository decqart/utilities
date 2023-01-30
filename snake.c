#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <curses.h>

int max_x, max_y;

int score = 0;
int body_size = 1;

bool quit = false;

typedef struct {
    int x;
    int y;
} vec;

int apple_x, apple_y;
bool change_apple = true;

void apple(void)
{
    if (change_apple)
    {
        srand(time(NULL));
    
        apple_x = (rand() % (max_x-2)) + 1;
        apple_y = rand() % (max_y-1);
        change_apple = false;
    }
    mvprintw(apple_y, apple_x, "A");
}

int snake_y = 2;
int snake_x = 2;
vec snke_dir = { .x=1, .y=0 };
vec body_pos[200];
int pos_count = 0;

void render_stuff(void)
{
    body_pos[pos_count].x = snake_x-snke_dir.x;
    body_pos[pos_count].y = snake_y-snke_dir.y;
    pos_count++;
    if (body_size-1 < pos_count)
        pos_count = 0;

    mvprintw(snake_y, snake_x, "O");
    
    for (int i = 0; i < body_size; ++i)
    {
        mvprintw(body_pos[i].y, body_pos[i].x, "O");

        if (snake_x == body_pos[i].x &&
            snake_y == body_pos[i].y)
            quit = true;
    }
    mvprintw(0, 0, "Score: %d", score);
}

void snake(void)
{
    render_stuff();
    switch(getch())
    {
    case 'W':
    case 'w':
        snke_dir.x = 0;
        if (snke_dir.y != 1)
            snke_dir.y = -1;
        break;
    case 'A':
    case 'a':
        if (snke_dir.x != 1)
            snke_dir.x = -1;
        snke_dir.y = 0;
        break;
    case 'S':
    case 's':
        snke_dir.x = 0;
        if (snke_dir.y != -1)
            snke_dir.y = 1;
        break;
    case 'D':
    case 'd':
        if (snke_dir.x != -1)
            snke_dir.x = 1;
        snke_dir.y = 0;
        break;
    case 'Q':
    case 'q':
        quit = true;
        break;
    default:
        break;
    }
    snake_x += snke_dir.x;
    snake_y += snke_dir.y;

    if (snake_y == apple_y && snake_x == apple_x)
    {
        change_apple = true;
        score++;
        if (score == 200)
        {
            endwin();
            printf("You WON!");
        }
        body_size++;
    }
    
    if (snke_dir.y != 0)
        usleep(80000);
    usleep(100000);

    if (snake_y < 0 || snake_x < 0 || snake_x > max_x || snake_y > max_y)
        quit = true;
}

int main(void)
{   
    initscr();
    raw();
    curs_set(0);
    noecho();
    nodelay(stdscr, 1);
    getmaxyx(stdscr, max_y, max_x);

    while (!quit)
    {
        apple();
        snake();
        clear();
    }
    endwin();
    printf("Score: %d\n", score);
    printf("GAME OVER\n");
    return 0;
}
