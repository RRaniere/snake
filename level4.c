#include "levels.h"
#include <time.h>
#include <stdlib.h>

void load_level4() {
    srand(time(NULL));

    if (obstacle_count == NULL) {
        obstacle_count = (int*)malloc(sizeof(int));
        if (obstacle_count == NULL) {
            exit(EXIT_FAILURE);
        }
    }
    *obstacle_count = 12;

    if (obstacles != NULL) {
        free(obstacles);
        obstacles = NULL;
    }

    obstacles = (vec2*)malloc((*obstacle_count) * sizeof(vec2));
    if (obstacles == NULL) {
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < *obstacle_count; i++) {
        obstacles[i].x = 1 + rand() % (screen_width - 2);
        obstacles[i].y = 1 + rand() % (screen_height - 2);

        while (collide(head, obstacles[i]) || collide_snake_body(obstacles[i])) {
            obstacles[i].x = 1 + rand() % (screen_width - 2);
            obstacles[i].y = 1 + rand() % (screen_height - 2);
        }
    }

    frame_time = 70000;
}
