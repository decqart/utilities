#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
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
    attron(COLOR_PAIR(2));
    mvaddch(apple_y, apple_x, 'A');
    attroff(COLOR_PAIR(2));
}

int snake_y = 2;
int snake_x = 2;
vec snke_dir = { .x=1, .y=0 };
vec body_pos[256];
int pos_count = 0;

void render_stuff(void)
{
    body_pos[pos_count].x = snake_x-snke_dir.x;
    body_pos[pos_count].y = snake_y-snke_dir.y;
    pos_count++;
    if (body_size-1 < pos_count)
        pos_count = 0;

    attron(COLOR_PAIR(1));
    mvaddch(snake_y, snake_x, 'O');

    for (int i = 0; i < body_size; ++i)
    {
        mvaddch(body_pos[i].y, body_pos[i].x, 'O');

        if (snake_x == body_pos[i].x &&
            snake_y == body_pos[i].y)
            quit = true;
    }
    attroff(COLOR_PAIR(1));
    mvprintw(0, 0, "Score: %d", score);
}

void snake(void)
{
    render_stuff();
    switch(tolower(getch()))
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
    case KEY_RESIZE:
        getmaxyx(stdscr, max_y, max_x);
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
            puts("You WON!");
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

    start_color();

    init_pair(1, COLOR_GREEN, COLOR_GREEN);
    init_pair(2, COLOR_RED, COLOR_RED);

    while (!quit)
    {
        apple();
        snake();
        clear();
    }
    endwin();
    printf("Score: %d\n", score);
    puts("GAME OVER");
    return 0;
}
