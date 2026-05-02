#include <stdio.h>
#include <math.h>
#include <unistd.h> // usleep
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <fcntl.h>

#define SCREEN_WIDTH 120
#define SCREEN_HEIGHT 40

#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define CELL_SIZE 64
#define FOV 60.0
#define STEP_SIZE 0.05

typedef struct {
    double x, y;
} Vector2D;

typedef struct {
    Vector2D position;
    double angle;
} Player;

struct termios orig_termios;

int world_map[MAP_HEIGHT][MAP_WIDTH] = {
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,2,2,2,2,2,2,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,2,0,0,0,0,2,0,0,0,3,3,3,3,3,3,3,3,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,2,0,0,0,0,2,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,2,2,0,2,2,2,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,1,1,0,0,3,0,0,0,0,0,0,0,0,1},
    {1,2,2,2,2,2,2,0,3,3,3,3,3,3,0,0,1,1,0,0,2,2,2,2,2,2,2,2,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,1,1,0,0,0,2,2,2,2,0,0,3,3,3,3,0,0,0,1,1,1,1,0,0,1},
    {1,0,0,1,1,1,1,0,0,0,2,2,2,2,0,0,3,3,3,3,0,0,0,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,3,3,3,3,3,3,3,0,2,2,2,2,2,2,2,2,2,2,2,0,1,1,1,1,1,1,1,1,1},
    {1,0,0,0,0,0,0,3,0,2,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,0,2,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,1,1,0,0,3,0,2,0,0,3,3,3,3,0,0,0,1,0,1,0,0,2,2,0,0,0,1},
    {1,0,0,1,1,0,0,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,2,2,0,0,0,1},
    {1,0,0,1,1,0,0,3,0,2,0,0,3,0,0,3,0,0,0,1,0,1,0,0,2,2,0,0,0,1},
    {1,0,0,0,0,0,0,3,0,2,0,0,3,3,3,3,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,3,0,2,0,0,0,0,0,0,0,0,0,1,0,1,0,0,0,0,0,0,0,1},
    {1,0,0,3,3,3,3,3,0,2,2,2,2,2,2,2,2,2,2,2,0,1,1,1,1,1,1,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1},
    {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1}
};

void set_nonblocking_mode(struct termios *old_t) {
    struct termios new_t;
    tcgetattr(STDIN_FILENO, old_t); 
    new_t = *old_t;
    
    new_t.c_lflag &= ~(ICANON | ECHO); 
    tcsetattr(STDIN_FILENO, TCSANOW, &new_t);
}

void reset_terminal_mode(struct termios *old_t) {
    tcsetattr(STDIN_FILENO, TCSANOW, old_t);
}

double deg_to_rad(double d) {
    return d * M_PI / 180.0;
}

double cast_ray(Player *p, double angle) {
    double dist = 0;

    while (dist < 2000) {
        double x = p->position.x + cos(angle) * dist;
        double y = p->position.y + sin(angle) * dist;

        int mx = (int)(x / CELL_SIZE);
        int my = (int)(y / CELL_SIZE);

        if (mx < 0 || mx >= MAP_WIDTH || my < 0 || my >= MAP_HEIGHT)
            break;

        if (world_map[my][mx] > 0)
            return dist;

        dist += STEP_SIZE;
    }

    return 2000;
}

char shades[] = " .:-=+*#%@";

int main() {
    Player player = {{100.0, 100.0}, 90.0};

    char screen[SCREEN_HEIGHT][SCREEN_WIDTH + 1];

    struct termios old_t;
    set_nonblocking_mode(&old_t);

    // Set stdin to non-blocking using fcntl
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        char ch;
        if (read(STDIN_FILENO, &ch, 1) > 0) {
            if (ch == 'q') break;
            else if (ch == 'a') {
                player.angle -= 5;
                if (player.angle < 0) player.angle += 360;
            }
            else if (ch == 'd') {
                player.angle += 5;
                if (player.angle >= 360) player.angle -= 360;
            }
            else if (ch == 'w') {
                player.position.x += cos(deg_to_rad(player.angle)) * 30;
                player.position.y += sin(deg_to_rad(player.angle)) * 30;
            }
            else if (ch == 's') {
                player.position.x -= cos(deg_to_rad(player.angle)) * 30;
                player.position.y -= sin(deg_to_rad(player.angle)) * 30;
            }
            fflush(stdout);
        }

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                screen[y][x] = ' ';
            }
            screen[y][SCREEN_WIDTH] = '\0';
        }

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            double ray_angle =
                deg_to_rad(player.angle)
                - deg_to_rad(FOV / 2)
                + ((double)x / SCREEN_WIDTH) * 
                deg_to_rad(FOV);

            double dist = cast_ray(&player, ray_angle);

            double corrected = dist * cos(ray_angle - deg_to_rad(player.angle));

            int wall_height = (int)(SCREEN_HEIGHT * CELL_SIZE / corrected);

            int start = SCREEN_HEIGHT / 2 - wall_height / 2;
            int end   = SCREEN_HEIGHT / 2 + wall_height / 2;

            if (start < 0) start = 0;
            if (end >= SCREEN_HEIGHT) end = SCREEN_HEIGHT - 1;

            // shading based on distance
            int shade_index = (int)(dist / 2000.0 * 9);
            if (shade_index > 9) shade_index = 9;

            char c = shades[9 - shade_index];

            for (int y = start; y <= end; y++) {
                screen[y][x] = c;
            }
        }

        // clear terminal
        printf("\033[H"); 

        // draw buffer
        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            printf("%s\n", screen[y]);
        }

        usleep(16000);
    }

    reset_terminal_mode(&old_t);
    return 0;
}
