//
//  snake.c
//  Atividade Avaliativa
//
//  Created by Ramon Raniere on 12/11/24.
//

#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define MAX_SCORE 255

int CURRENT_LEVEL = 1;
int FRAME_TIME = 110000;
int screen_width = 50;
int screen_height = 40;

typedef struct {
    int x, y;
} vec2;

vec2 head = {0, 0};
vec2 segments[MAX_SCORE + 1];
vec2 dir = {1, 0};
vec2 berry, specialBerry;

int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;

WINDOW *win;

void reset_game_state() {
    head = (vec2){0, 0};
    dir = (vec2){1, 0};
    score = 0;
    is_running = true;
    sprintf(score_message, "[ Score: %d ]", score);
}

void quit_game() {
    endwin();
    printf("\e[1;1H\e[2j]]\e[?25h");
    exit(0);
}

void initialize_terminal() {
    win = initscr();
    keypad(win, true);
    noecho();
    nodelay(win, true);
    curs_set(0);

    if (!has_colors()) {
        endwin();
        fprintf(stderr, "Your terminal does not support color\n");
        exit(1);
    }

    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);

    srand(time(NULL));
}

void spawn_random_berries() {
    berry = (vec2){rand() % screen_width, rand() % screen_height};
    specialBerry = (vec2){rand() % screen_width, rand() % screen_height};
}

void initialize_game() {
    initialize_terminal();
    spawn_random_berries();
    reset_game_state();
}

void process_input() {
    int pressed = wgetch(win);
    if (pressed == KEY_LEFT && dir.x != 1) dir = (vec2){-1, 0};
    else if (pressed == KEY_RIGHT && dir.x != -1) dir = (vec2){1, 0};
    else if (pressed == KEY_UP && dir.y != 1) dir = (vec2){0, -1};
    else if (pressed == KEY_DOWN && dir.y != -1) dir = (vec2){0, 1};
    else if (pressed == ' ') reset_game_state();
    else if (pressed == '\e') quit_game();
}

bool collide(vec2 a, vec2 b) {
    return a.x == b.x && a.y == b.y;
}

bool collide_snake_body(vec2 point) {
    for (int i = 0; i < score; i++) {
        if (collide(point, segments[i])) return true;
    }
    return false;
}

vec2 spawn_berry() {
    vec2 new_berry;
    do {
        new_berry = (vec2){rand() % (screen_width - 2) + 1, rand() % (screen_height - 2) + 1};
    } while (collide(head, new_berry) || collide_snake_body(new_berry));
    return new_berry;
}

void next_level() {
    mvaddstr(screen_height / 2, screen_width - 5, "NEXT LEVEL!");
    refresh();
    usleep(1500000);

    for (int i = 3; i >= 0; i--) {
        mvprintw(screen_height / 2, (screen_width - 3), i > 0 ? "%d..." : "GO!", i);
        refresh();
        usleep(1000000);
    }

    FRAME_TIME -= FRAME_TIME * 0.2;
}

void update() {
    for (int i = score; i > 0; i--) {
        segments[i] = segments[i - 1];
    }
    segments[0] = head;

    head.x += dir.x;
    head.y += dir.y;

    if (collide_snake_body(head) || head.x < 0 || head.y < 0 || head.x >= screen_width || head.y >= screen_height) {
        is_running = false;
    }

    if (collide(head, berry)) {
        if (score < MAX_SCORE) score++;
        if (score % 10 == 0) next_level();
        berry = spawn_berry();
    }

    if (collide(head, specialBerry)) {
        if (score < MAX_SCORE) score += 5;
        if (CURRENT_LEVEL * 10 <= score) next_level();
        specialBerry = spawn_berry();
    }

    sprintf(score_message, "[ Score: %d ]", score);
    usleep(FRAME_TIME);
}

void draw_border(int y, int x, int width, int height) {
    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width * 2 + 1, ACS_URCORNER);
    mvhline(y, x + 1, ACS_HLINE, width * 2);
    mvvline(y + 1, x, ACS_VLINE, height);
    mvvline(y + 1, x + width * 2 + 1, ACS_VLINE, height);
    mvaddch(y + height + 1, x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);
    mvhline(y + height + 1, x + 1, ACS_HLINE, width * 2);
}

void draw_game() {
    erase();
    mvaddch(berry.y + 1, berry.x * 2 + 1, ACS_DIAMOND);
    attron(COLOR_PAIR(1));
    mvaddch(specialBerry.y + 1, specialBerry.x * 2 + 1, ACS_DIAMOND);
    attroff(COLOR_PAIR(1));
    attron(COLOR_PAIR(2));
    for (int i = 0; i < score; i++) mvaddch(segments[i].y + 1, segments[i].x * 2 + 1, ACS_DIAMOND);
    mvaddch(head.y + 1, head.x * 2 + 1, 'O');
    attroff(COLOR_PAIR(2));
    attron(COLOR_PAIR(3));
    draw_border(0, 0, screen_width, screen_height);
    attroff(COLOR_PAIR(3));
    mvaddstr(0, screen_width - 5, score_message);
}

int main(void) {
    initialize_game();

    while (true) {
        process_input();
        if (skip) { skip = false; continue; }
        update();
        draw_game();
    }

    quit_game();
    return 0;
}
