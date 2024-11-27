#ifndef SNAKE_H
#define SNAKE_H

#include <stdbool.h>

#define MAX_SCORE 255
#define MAX_OBSTACLES 50

typedef struct {
    int x;
    int y;
} vec2;

extern int current_level;
extern int frame_time;
extern int scoreboard[3];
extern int score;
extern char score_message[16];
extern bool skip;
extern bool is_running;
extern bool showing_scoreboard;
extern int screen_width;
extern int screen_height;
extern int* obstacle_count; 
extern vec2 head;
extern vec2 segments[MAX_SCORE + 1];
extern vec2 dir;
extern vec2 berry;
extern vec2 specialBerry;
extern vec2* obstacles;

void init();
void restart_game();
void quit_game();
void process_input();
void menu();
bool collide(vec2 a, vec2 b);
bool collide_snake_body(vec2 point);
bool collide_obstacles(vec2 point);
void draw_border(int y, int x, int width, int height);
void set_scoreboard();
void get_scoreboard();
void game_over();
vec2 spawn_berry();
vec2 spawn_special_berry();
void next_level();
void update();
void draw();

#endif 
