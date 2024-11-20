//
//  snake.c
//  Atividade Avaliativa
//

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <windows.h>

#define MAX_SCORE 255

int CURRENT_LEVEL = 1;
int FRAME_TIME = 110000;
int SCORE = 0;

char SCORE_MESSAGE[16];
bool SKIP = false;
bool IS_RUNNING = true;

int SCREEN_WIDTH = 50;
int SCREEN_HEIGHT = 40;

WINDOW *WIN;

typedef struct {
    int x;
    int y;
} vec2;

vec2 HEAD = {0, 0};
vec2 SEGMENTS[MAX_SCORE + 1];
vec2 DIR = {1, 0};

vec2 BERRY;
vec2 SPECIAL_BERRY;

void custom_usleep(__int64 usec) {
    HANDLE timer;
    LARGE_INTEGER ft;

    ft.QuadPart = -(10 * usec);

    timer = CreateWaitableTimer(NULL, TRUE, NULL);
    SetWaitableTimer(timer, &ft, 0, NULL, NULL, 0);
    WaitForSingleObject(timer, INFINITE);
    CloseHandle(timer);
}

void restart_game() {
    HEAD.x = 0;
    HEAD.y = 0;
    DIR.x = 1; 
    DIR.y = 0;
    SCORE = 0;
    CURRENT_LEVEL = 1;
    FRAME_TIME = 110000;

    BERRY = (vec2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};
    SPECIAL_BERRY = (vec2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};

    snprintf(SCORE_MESSAGE, sizeof(SCORE_MESSAGE), "[ Score: %d ]", SCORE);
    IS_RUNNING = true;
}

void quit_game() {
    endwin();
    printf("\e[1;1H\e[2j]]");
    printf("\e[?25h");
    exit(0);
}

void init() {
    srand(time(NULL));
    WIN = initscr();

    keypad(WIN, true);
    noecho();
    nodelay(WIN, true);
    curs_set(0);

    if (has_colors() == FALSE) {
        endwin();
        fprintf(stderr, "Your terminal does not support color \n");
        exit(1);
    }

    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);

    BERRY = (vec2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};
    SPECIAL_BERRY = (vec2){rand() % SCREEN_WIDTH, rand() % SCREEN_HEIGHT};

    snprintf(SCORE_MESSAGE, sizeof(SCORE_MESSAGE), "[ Score: %d ]", SCORE);
}

void process_input() {
    int pressed = wgetch(WIN);

    if (pressed == KEY_LEFT) {
        if (DIR.x == 1) {
            SKIP = true;
            return;
        }
        DIR.x = -1;
        DIR.y = 0;
    }
    if (pressed == KEY_RIGHT) {
        if (DIR.x == -1) {
            SKIP = true;
            return;
        }
        DIR.x = 1;
        DIR.y = 0;
    }
    if (pressed == KEY_UP) {
        if (DIR.y == 1) {
            SKIP = true;
            return;
        }
        DIR.x = 0;
        DIR.y = -1;
    }
    if (pressed == KEY_DOWN) {
        if (DIR.y == -1) {
            SKIP = true;
            return;
        }
        DIR.x = 0;
        DIR.y = 1;
    }
    if (pressed == ' ') {
        if (!IS_RUNNING) {
            restart_game();
        }
    }
    if (pressed == '\e') {
        IS_RUNNING = false;
        quit_game();
    }
}

bool collide(vec2 a, vec2 b) {
    return (a.x == b.x && a.y == b.y);
}

bool collide_snake_body(vec2 point) {
    for (int i = 0; i < SCORE; i++) {
        if (collide(point, SEGMENTS[i])) {
            return true;
        }
    }
    return false;
}

void draw_border(int y, int x, int width, int height) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width * 2 + 1, ACS_URCORNER);

    for (int i = 1; i < width * 2 + 1; i++) {
        mvaddch(y, x + i, ACS_HLINE);
    }

    for (int i = 1; i < height + 1; i++) {
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width * 2 + 1, ACS_VLINE);
    }

    mvaddch(y + height + 1, x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);

    for (int i = 1; i < width * 2 + 1; i++) {
        mvaddch(y + height + 1, x + i, ACS_HLINE);
    }
}

void game_over() {
    while (IS_RUNNING == false) {
        process_input();

        mvaddstr(SCREEN_HEIGHT / 2, SCREEN_WIDTH - 16, "            Game Over           ");
        mvaddstr(SCREEN_HEIGHT / 2 + 3, SCREEN_WIDTH - 16, "[SPACE] to restart, [ESC] to quit");

        custom_usleep(FRAME_TIME);
    }
}

vec2 spawn_fruit(vec2 fruit) {
    do {
        fruit.x = 1 + rand() % (SCREEN_WIDTH - 2);
        fruit.y = 1 + rand() % (SCREEN_HEIGHT - 2);
    } while (collide(HEAD, fruit) || collide_snake_body(fruit));
    return fruit;
}

void handle_fruit_collision(vec2 *fruit, int points) {
    if (collide(HEAD, *fruit)) {
        if (SCORE < MAX_SCORE) {
            SCORE += points;
            snprintf(SCORE_MESSAGE, sizeof(SCORE_MESSAGE), "[ Score: %d ]", SCORE);
        }

        if (SCORE >= CURRENT_LEVEL * 10) {
            CURRENT_LEVEL++;
            mvaddstr(SCREEN_HEIGHT / 2, SCREEN_WIDTH - 5, "NEXT LEVEL!");
            refresh();
            custom_usleep(1500000);
            FRAME_TIME = FRAME_TIME * 0.8 > 30000 ? FRAME_TIME * 0.8 : 30000;
        }

        *fruit = spawn_fruit(*fruit);
    }
}

void update() {
    for (int i = SCORE; i > 0; i--) {
        SEGMENTS[i] = SEGMENTS[i - 1];
    }
    SEGMENTS[0] = HEAD;

    HEAD.x += DIR.x;
    HEAD.y += DIR.y;

    if (collide_snake_body(HEAD) || HEAD.x < 0 || HEAD.y < 0 || HEAD.x >= SCREEN_WIDTH || HEAD.y >= SCREEN_HEIGHT) {
        IS_RUNNING = false;
        return;
    }

    handle_fruit_collision(&BERRY, 1);
    handle_fruit_collision(&SPECIAL_BERRY, 5);

    custom_usleep(FRAME_TIME);
}

void draw() {
    erase();

    mvaddch(BERRY.y + 1, BERRY.x * 2 + 1, ACS_DIAMOND);

    attron(COLOR_PAIR(1));
    mvaddch(SPECIAL_BERRY.y + 1, SPECIAL_BERRY.x * 2 + 1, ACS_DIAMOND);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for (int i = 0; i < SCORE; i++) {
        mvaddch(SEGMENTS[i].y + 1, SEGMENTS[i].x * 2 + 1, ACS_DIAMOND);
    }
    mvaddch(HEAD.y + 1, HEAD.x * 2 + 1, 'O');
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    draw_border(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    attroff(COLOR_PAIR(3));
    mvaddstr(0, SCREEN_WIDTH - 5, SCORE_MESSAGE);
}

int main(void) {
    init();

    while (true) {
        process_input();
        if (SKIP == true) {
            SKIP = false;
            continue;
        }

        update();
        draw();
    }

    quit_game();
    return 0;
}