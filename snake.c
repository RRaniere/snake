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

int current_level = 1;
int frame_time = 110000;

int score = 0;
char score_message[16];

bool skip = false;
bool is_running = true;

int screen_width = 50;
int screen_height = 40;

WINDOW *win;

typedef struct {
    
    int x;
    int y;
    
} vec2;

vec2 head = { 0 , 0 };
vec2 segments[MAX_SCORE + 1];
vec2 dir = { 1 , 0 };

vec2 berry;
vec2 specialBerry;

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

}



void quit_game() { 

    endwin();
    printf("\e[1;1H\e[2j]]");
    printf("\e[?25h");

    exit(0);

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

    sprintf(score_message, "[Score: %d ]", score);

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

void game_over() { 

    while(is_running == false) { 
        process_input();

        mvaddstr(screen_height / 2  , screen_width - 16, "            Game Over           ");
        mvaddstr(screen_height / 2 + 3, screen_width - 16, "[SPACE] to restart, [ESC] to quit");

        usleep(frame_time);

    }

}



vec2 spawn_berry() { 

    vec2 berry = { 1 + rand() % (screen_width - 2 ), 1 + rand() % (screen_height - 2)};
    while(collide(head, berry) || collide_snake_body(berry)) { 
        berry.x = 1 + rand() % (screen_width - 2); 
        berry.y = 1 + rand() % (screen_height - 2); 
    }
    return berry;

}

vec2 spawn_special_berry() { 

    vec2 specialBerry = { 1 + rand() % (screen_width - 2 ), 1 + rand() % (screen_height - 2)};
    while(collide(head, specialBerry) || collide_snake_body(specialBerry)) { 
        specialBerry.x = 1 + rand() % (screen_width - 2); 
        specialBerry.y = 1 + rand() % (screen_height - 2); 
    }
    return specialBerry;

}



void next_level() {

    mvaddstr(screen_height / 2, (screen_width - 5), "NEXT LEVEL!");
    refresh(); 
    usleep(1500000);  
    mvaddstr(screen_height / 2, (screen_width - 5), "           ");

    for(int i = 3; i >= 0; i--) { 
        char countdown_message[5];
        
        if(i > 0) { 
            snprintf(countdown_message, sizeof(countdown_message), "%d...", i);
            mvaddstr(screen_height / 2, (screen_width - 3), countdown_message);
        }else{ 
            mvaddstr(screen_height / 2, (screen_width - 3), "GO!");
        }

        refresh(); 
        usleep(1000000); 

    }
    
    refresh();

    frame_time = frame_time  - (frame_time  * 0.2);
    
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
        if(score % 10 == 0) { 
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

        if(current_level * 10 <= score) { 
            current_level++;
            next_level();  
        }

        specialBerry = spawn_special_berry();

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
        mvaddch(segments[i].y + 1, segments[i].x * 2 +1, ACS_DIAMOND);
    }
    mvaddch(head.y+1, head.x * 2+1, 'O');
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(3));
    draw_border(0,0, screen_width, screen_height);
    attroff(COLOR_PAIR(3));
    mvaddstr(0, screen_width - 5, score_message);

}


int main(void) {
    
    init();

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
