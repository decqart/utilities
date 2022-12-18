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

typedef vec pos;

int apple_x, apple_y;
bool change_apple = true;

void apple(void)
{
    if (change_apple)
    {
        srand(time(NULL));
    
        apple_x = rand() % (max_x-1);
        apple_y = rand() % (max_y-1);
        change_apple = false;
    }
    mvprintw(apple_y, apple_x, "A");
}

int snake_y = 2;
int snake_x = 2;
vec snke_dir = { .x=1, .y=0 };
pos body_pos[200];
int pos_count = 0;

void render_snake(void)
{
    body_pos[pos_count].x = snake_x;
    body_pos[pos_count].y = snake_y;
    pos_count++;
    if (body_size-1 < pos_count)
        pos_count = 0;
    
    for (int i = 0; i <= body_size-1; i++)
    {
        mvprintw(body_pos[i].y, body_pos[i].x, "O");
        for (int j = i + 1; j < body_size; j++)
        {
            if (body_pos[i].x == body_pos[j].x &&
                body_pos[i].y == body_pos[j].y)
            {
                endwin();
                printf("Score: %d\n", score);
                printf("died of body\n");
                printf("GAME OVER\n");
                exit(0);
            }
        }
    }
}

void snake(void)
{
    render_snake();
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
        body_size++;
    }
    
    if (snke_dir.y != 0)
        usleep(80000);

    if (snake_y < 0 || snake_x < 0 || snake_x > max_x || snake_y > max_y)
    {
        endwin();
        printf("Score: %d\n", score);
        printf("GAME OVER\n");
        exit(0);
    }
}

int main(void)
{   
    initscr();
    raw();
    curs_set(0);
    noecho();
    nodelay(stdscr, 1);
    getmaxyx(stdscr, max_y, max_x);
    // box(stdscr, ACS_VLINE, ACS_HLINE);

    while (!quit)
    {
        mvprintw(0, 0, "Score: %d", score);
        apple();
        snake();
        usleep(100000);
        clear();
    }
    endwin();
    return 0;
}

