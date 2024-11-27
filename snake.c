//
//  snake.c
//  Atividade Avaliativa
//
//  Created by Ramon Raniere on 12/11/24.
//

#include "levels.h"
#include <curses.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
 
WINDOW *win;

int current_level = 1;
int frame_time = 110000;

int scoreboard[3];
int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;
bool showing_scoreboard = true;

int screen_width = 50;
int screen_height = 40;

vec2 head = { 0 , 0 };
vec2 segments[MAX_SCORE + 1];
vec2 dir = { 1 , 0 };

vec2 berry;
vec2 specialBerry;

vec2 obstacles[MAX_OBSTACLES];
int obstacle_count;

void restart_game() { 

    head.x = 0;
    head.y = 0;
    dir.x = 1; 
    dir.y = 0;
    score = 0;
    sprintf(score_message, "[ Score: %d ]", score);
    is_running = true;
    current_level = 1;
    frame_time = 110000;
    obstacle_count = 0;

}

void quit_game() { 

    endwin();
    printf("\e[1;1H\e[2j]]");
    printf("\e[?25h");

    exit(0);

}

void load_level() {
    switch (current_level) {
        case 2:
            load_level2();
            break;
        case 3:
            load_level3();
            break; 
        case 4:
            load_level4();
            break;
        case 5:
            load_level5();
            break;
        case 6:
            frame_time = 65000;
            break;
        default:
            break;
    }
}

void init() { 

    srand(time(NULL));

    win = initscr();

    keypad(win, true);
    noecho();
    nodelay(win, true);
    curs_set(0);

    if(has_colors() == FALSE) { 
        endwin();
        fprintf(stderr, "Your terminal does not support color \n");
        exit(1);
    }

    start_color();
    use_default_colors();
    init_pair(1, COLOR_RED, -1);
    init_pair(2, COLOR_GREEN, -1);
    init_pair(3, COLOR_YELLOW, -1);

    berry.x = rand() % screen_width;
    berry.y = rand() % screen_height;

    specialBerry.x = rand() % screen_width;
    specialBerry.y = rand() % screen_height;

    sprintf(score_message, "[ Score: %d ]", score);
}

void process_input() { 

    int pressed = wgetch(win);
    if(pressed == KEY_LEFT) { 
        if(dir.x == 1) { 
            return;
            skip = true;
        }
        dir.x = -1;
        dir.y = 0;
    }
    if(pressed == KEY_RIGHT) { 
        if(dir.x == -1) { 
            return;
            skip = true;
        }
        dir.x = 1;
        dir.y = 0;
    }
    if(pressed == KEY_UP) { 
        if(dir.y == 1) { 
            return;
            skip = true;
        }
        dir.x = 0;
        dir.y = -1;
    }
    if(pressed == KEY_DOWN) { 
        if(dir.y == -1) { 
            return;
            skip = true;
        }
        dir.x = 0;
        dir.y = 1;
    }
    if(pressed ==  ' ') {
        if(!is_running) { 
            restart_game();
        }
        
    }
    if(pressed ==  '\e') {
       is_running = false;
       quit_game();
    }
    if (pressed == '\n' && !is_running) {  

         while(is_running == false) { 

            process_input();

                mvaddstr(screen_height / 2  , screen_width - 5, "                 ");
                mvaddstr(screen_height / 2 + 3, screen_width - 14, "                                 ");

                mvaddstr(screen_height / 3, screen_width - 16, "            SCOREBOARD           ");

                mvprintw(screen_height / 3 + 3, screen_width - 6, "1º Place - %d", scoreboard[0]);
                mvprintw(screen_height / 3 + 4, screen_width - 6, "2º Place - %d", scoreboard[1]);
                mvprintw(screen_height / 3 + 5, screen_width - 6, "3º Place - %d", scoreboard[2]);

                mvaddstr(screen_height / 2 + 3, screen_width - 15, "[SPACE] to restart, [ESC] to quit");

            usleep(frame_time);

        }
        
    }

}


void menu() { 

    is_running = false;

    while(is_running == false) { 

        process_input();

        mvaddstr(screen_height / 2  , screen_width - 5, "BINARY SNAKE");
        mvaddstr(screen_height / 2 + 3, screen_width - 14, "[SPACE] to play, [ENTER] to score");

        usleep(frame_time);

    }

}

bool collide(vec2 a, vec2 b) { 

    if(a.x == b.x && a.y == b.y) { 
        return true;
    }
    return false;

}

bool collide_snake_body(vec2 point) { 

    for(int i = 0; i < score; i++) { 

        if(collide(point, segments[i])) { 
            return true;
        }

    }
   return false;

}

bool collide_obstacles(vec2 point) {
    for (int i = 0; i < obstacle_count; i++) {
        if (collide(point, obstacles[i])) {
            return true;
        }
    }
    return false;
}

void draw_border(int y, int x, int width, int height) { 

    mvaddch(y, x, ACS_ULCORNER);
    mvaddch(y, x + width * 2 + 1 , ACS_URCORNER);

    for(int i = 1; i < width * 2 + 1; i++) { 
        mvaddch(y, x + i, ACS_HLINE);
    }

    for(int i = 1; i < height + 1; i++) { 
        mvaddch(y + i, x, ACS_VLINE);
        mvaddch(y + i, x + width * 2 + 1, ACS_VLINE);
    }

    mvaddch(y + height + 1,x, ACS_LLCORNER);
    mvaddch(y + height + 1, x + width * 2 + 1, ACS_LRCORNER);
    for(int i = 1; i < width * 2 + 1; i++) { 
        mvaddch(y + height + 1, x + i, ACS_HLINE);
    }

}

void set_scoreboard() {

    FILE *file = fopen("scoreboard", "wb");
    if (file == NULL) {
        printf("Error opening file for writing");
        return;
    }
    for (int i = 3 - 1; i >= 0; i--) {
        if (score > scoreboard[i]) {
            if (i < 3 - 1 ) {
                scoreboard[i + 1] = scoreboard[i];
            }
            scoreboard[i] = score;
        } else {
            break;
        }
    }

    fwrite(scoreboard, sizeof(int), 3, file); 
    fclose(file);
}

void get_scoreboard() {
    FILE *file = fopen("scoreboard", "rb"); 
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }
    fread(scoreboard, sizeof(int), 3, file); 
    fclose(file); 
}

void game_over() { 

    set_scoreboard();

    while(is_running == false) { 

        process_input();

        mvaddstr(screen_height / 3, screen_width - 16, "            SCOREBOARD           ");

        mvprintw(screen_height / 3 + 3, screen_width - 6, "1º Place - %d", scoreboard[0]);
        mvprintw(screen_height / 3 + 4, screen_width - 6, "2º Place - %d", scoreboard[1]);
        mvprintw(screen_height / 3 + 5, screen_width - 6, "3º Place - %d", scoreboard[2]);

        mvaddstr(screen_height / 2 + 3, screen_width - 15, "[SPACE] to restart, [ESC] to quit");

        usleep(frame_time);

    }

}

vec2 spawn_berry() { 

    vec2 berry = { 1 + rand() % (screen_width - 2 ), 1 + rand() % (screen_height - 2)};
    while(collide(head, berry) || collide_snake_body(berry) || collide_obstacles(berry)) { 
        berry.x = 1 + rand() % (screen_width - 2); 
        berry.y = 1 + rand() % (screen_height - 2); 
    }
    return berry;

}

vec2 spawn_special_berry() { 

    vec2 specialBerry = { 1 + rand() % (screen_width - 2 ), 1 + rand() % (screen_height - 2)};
    while(collide(head, specialBerry) || collide_snake_body(specialBerry) || collide_obstacles(berry)) { 
        specialBerry.x = 1 + rand() % (screen_width - 2); 
        specialBerry.y = 1 + rand() % (screen_height - 2); 
    }
    return specialBerry;

}

void next_level() {

    load_level();
    draw();

    mvaddstr(screen_height / 2, (screen_width - 3), "NEXT LEVEL!");
    refresh(); 
    usleep(1500000);  
    mvaddstr(screen_height / 2, (screen_width - 3), "                         ");

    for(int i = 3; i >= 0; i--) { 
        char countdown_message[5];
        
        if(i > 0) { 
            snprintf(countdown_message, sizeof(countdown_message), "%d...", i);
            mvaddstr(screen_height / 2, (screen_width - 0), countdown_message);
        }else{ 
            mvaddstr(screen_height / 2, (screen_width - 0), "GO!");
        }

        refresh(); 
        usleep(1000000); 

    }
    
    refresh();
}


void update() { 

    for(int i = score; i > 0; i--) { 
        segments[i] =  segments[i - 1];
    }  
    segments[0] = head;
    
    head.x += dir.x;
    head.y += dir.y;

    if(collide_snake_body(head) || head.x < 0 || head.y < 0 || head.x >= screen_width || head.y >= screen_height ) { 
        is_running = false;
        game_over();
    } 

    if(collide(head, berry)) { 

        if(score < MAX_SCORE) { 
            score += 1;
            sprintf(score_message, "[ Score : %d ]", score);
        }
        if(score % 10 == 0 && current_level < 6) { 
            current_level++;
            next_level();  
        }

        berry = spawn_berry();
    }

    if(collide(head, specialBerry)) { 

        if(score < MAX_SCORE) { 
            score += 5;
            sprintf(score_message, "[ Score : %d ]", score);
        }

        if(current_level * 10 <= score && current_level < 6) { 
            current_level++;
            next_level();  
        }

        specialBerry = spawn_special_berry();

    }

    if (collide_obstacles(head)) {
        is_running = false;
        game_over();
    }

    usleep(frame_time);

}


void draw() { 

    erase();

    mvaddch(berry.y+1, berry.x * 2+1, ACS_DIAMOND);

    attron(COLOR_PAIR(1));
    mvaddch(specialBerry.y+1, specialBerry.x * 2+1, ACS_DIAMOND);
    attroff(COLOR_PAIR(1));

    attron(COLOR_PAIR(2));
    for(int i = 0; i < score; i++) { 

        if(rand() % 2 == 0) { 
            mvaddch(segments[i].y + 1, segments[i].x * 2 +1, 'O');
        }else{ 
            mvaddch(segments[i].y + 1, segments[i].x * 2 +1, '1');
        }

    }
    mvaddch(head.y+1, head.x * 2+1, 'O');
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    draw_border(0,0, screen_width, screen_height);
    attroff(COLOR_PAIR(3));
    mvaddstr(0, screen_width - 5, score_message);

    attron(COLOR_PAIR(2));
    for (int i = 0; i < obstacle_count; i++) {
            mvaddch(obstacles[i].y + 1, obstacles[i].x *2+1, ACS_BLOCK); 
    }
    attroff(COLOR_PAIR(2));

}

int main(void) {
    
    get_scoreboard();
    init();
    menu();

    while (true) {
        
        process_input();
        if(skip == true) { 
            skip = false;
            continue;
        }

        update();
        draw();

    }
    
    quit_game();

    return 0;
    
}
