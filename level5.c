#include "levels.h"
#include <time.h>
#include <stdlib.h>

void load_level5() {

    srand(time(NULL));

    obstacle_count = 15;

    for(int i = 0; i <= obstacle_count; i++) { 

        obstacles[i].x = 1 + rand() % (screen_width - 2 );
        obstacles[i].y = 1 + rand() % (screen_height - 2);

        while(collide(head, obstacles[i]) || collide_snake_body(obstacles[i])) { 
            obstacles[i].x = 1 + rand() % (screen_width - 2); 
            obstacles[i].y = 1 + rand() % (screen_height - 2); 
        }

    }

    frame_time = 70000;


} 
 