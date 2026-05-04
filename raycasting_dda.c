#include <stdio.h>
#include <SDL.h>
#include <math.h>

#define WINDOW_TITLE "Raycasting"
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
#define BACKGROUND_COLOR 0x00000000
#define TARGET_FPS 60
#define MAP_WIDTH 30
#define MAP_HEIGHT 30
#define CELL_SIZE 64
#define STEP_SIZE 0.05
#define FOV 60.0
#define MAX_DISTANCE (sqrt(pow(WINDOW_WIDTH, 2) + pow(WINDOW_HEIGHT, 2)) + 1000)
#define TEX_WIDTH 64
#define TEX_HEIGHT 64

typedef struct {
    double x, y;
} Vector2D;

typedef struct {
    Vector2D position;
    double angle;
} Player;

typedef struct {
    double distance;
    int hit_wall;
    int side; // 0 for X, 1 for Y
    double wall_x;
} RayResult;

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

double deg_to_rad(double degrees) {
    return degrees * M_PI / 180.0;
}

RayResult cast_ray_dda(Player *player, double ray_angle) {
    RayResult result = {0, 0, 0, 0};
    double ray_dir_x = cos(ray_angle);
    double ray_dir_y = sin(ray_angle);

    // Convert player position to map-grid units
    double pos_x = player->position.x / CELL_SIZE;
    double pos_y = player->position.y / CELL_SIZE;

    int map_x = (int)pos_x;
    int map_y = (int)pos_y;

    // 1e30 to avoid division by zero
    double delta_dist_x = (ray_dir_x == 0) ? 1e30 : fabs(1.0 / ray_dir_x);
    double delta_dist_y = (ray_dir_y == 0) ? 1e30 : fabs(1.0 / ray_dir_y);

    int step_x, step_y;
    double side_dist_x, side_dist_y;

    if (ray_dir_x < 0) {
        step_x = -1;
        side_dist_x = (pos_x - map_x) * delta_dist_x;
    } else {
        step_x = 1;
        side_dist_x = (map_x + 1.0 - pos_x) * delta_dist_x;
    }

    if (ray_dir_y < 0) {
        step_y = -1;
        side_dist_y = (pos_y - map_y) * delta_dist_y;
    } else {
        step_y = 1;
        side_dist_y = (map_y + 1.0 - pos_y) * delta_dist_y;
    }

    while (1) {
        if (side_dist_x < side_dist_y) {
            side_dist_x += delta_dist_x;
            map_x += step_x;
            result.side = 0;
        } else {
            side_dist_y += delta_dist_y;
            map_y += step_y;
            result.side = 1;
        }

        if (world_map[map_y][map_x] > 0) {
            result.hit_wall = world_map[map_y][map_x];
            break;
        }
    
        // default max distance if no wall is hit
        if (map_x < 0 || map_x >= MAP_WIDTH || map_y < 0 || map_y >= MAP_HEIGHT) break;
    }

    if (result.side == 0) {
        result.distance = (side_dist_x - delta_dist_x) * CELL_SIZE;
        result.wall_x = pos_y + result.distance / CELL_SIZE * ray_dir_y;
    } else {
        result.distance = (side_dist_y - delta_dist_y) * CELL_SIZE;
        result.wall_x = pos_x + result.distance / CELL_SIZE * ray_dir_x;
    }
    result.wall_x -= floor(result.wall_x); // fractional part only for texture mapping
    return result;
}


void draw_line(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, Uint32 color, double shading) {
    Uint8 r = (color >> 24) & 0xFF;
    Uint8 g = (color >> 16) & 0xFF;
    Uint8 b = (color >> 8) & 0xFF;
    Uint8 a = color & 0xFF;
    SDL_SetRenderDrawColor(renderer, r * shading, g * shading, b * shading, a);
    SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void draw_line_textured(SDL_Renderer *renderer, RayResult result, int tex_x, double step, int x1, int y1, int x2, int y2, Uint32 texture[TEX_WIDTH][TEX_HEIGHT], double tex_pos, double shading) {
    tex_x &= (TEX_WIDTH - 1);
    for (int y = y1; y <= y2; y++) {
        int tex_y = (int)tex_pos & (TEX_HEIGHT - 1);
        tex_pos += step;
        
        Uint32 color = texture[tex_x][tex_y];
        
        Uint8 r = ((color >> 24) & 0xFF) * shading;
        Uint8 g = ((color >> 16) & 0xFF) * shading;
        Uint8 b = ((color >> 8) & 0xFF) * shading;
        Uint8 a = (color & 0xFF) * shading;
        
        SDL_SetRenderDrawColor(renderer, r, g, b, a);
        SDL_RenderDrawPoint(renderer, x1, y);
    }
}

int main() {
    SDL_Window *window = SDL_CreateWindow(
        WINDOW_TITLE,
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WINDOW_WIDTH,
        WINDOW_HEIGHT, 
        0
    );

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderClear(renderer);

    SDL_Surface* loadedSurface = SDL_LoadBMP("texture-1.bmp");
    SDL_Surface* formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(loadedSurface);

    int width = formattedSurface->w;
    int height = formattedSurface->h;
    Uint32* pixels = (Uint32*)formattedSurface->pixels;

    Uint32 texture_1[TEX_WIDTH][TEX_HEIGHT];
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            int src_x = (x * width) / TEX_WIDTH;
            int src_y = (y * height) / TEX_HEIGHT;
            texture_1[x][y] = pixels[src_y * width + src_x];
        }
    }
    SDL_FreeSurface(formattedSurface);

    loadedSurface = SDL_LoadBMP("texture-2.bmp");
    formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(loadedSurface);

    width = formattedSurface->w;
    height = formattedSurface->h;
    pixels = (Uint32*)formattedSurface->pixels;

    Uint32 texture_2[TEX_WIDTH][TEX_HEIGHT];
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            int src_x = (x * width) / TEX_WIDTH;
            int src_y = (y * height) / TEX_HEIGHT;
            texture_2[x][y] = pixels[src_y * width + src_x];
        }
    }
    SDL_FreeSurface(formattedSurface);

    loadedSurface = SDL_LoadBMP("texture-3.bmp");
    formattedSurface = SDL_ConvertSurfaceFormat(loadedSurface, SDL_PIXELFORMAT_RGBA8888, 0);
    SDL_FreeSurface(loadedSurface);

    width = formattedSurface->w;
    height = formattedSurface->h;
    pixels = (Uint32*)formattedSurface->pixels;

    Uint32 texture_3[TEX_WIDTH][TEX_HEIGHT];
    for (int y = 0; y < TEX_HEIGHT; y++) {
        for (int x = 0; x < TEX_WIDTH; x++) {
            int src_x = (x * width) / TEX_WIDTH;
            int src_y = (y * height) / TEX_HEIGHT;
            texture_3[x][y] = pixels[src_y * width + src_x];
        }
    }
    SDL_FreeSurface(formattedSurface);

    Player player = { { 100.0, 100.0 }, 90.0 };

    int delay = 1000 / TARGET_FPS;
    int done = 0;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                done = 1;
            } else if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_a:
                    case SDLK_LEFT:
                        player.position.x += cos(deg_to_rad(player.angle - 90)) * 15.0;
                        player.position.y += sin(deg_to_rad(player.angle - 90)) * 15.0;
                        break;
                    case SDLK_d:
                    case SDLK_RIGHT:
                        player.position.x -= cos(deg_to_rad(player.angle - 90)) * 15.0;
                        player.position.y -= sin(deg_to_rad(player.angle - 90)) * 15.0;
                        break;
                    case SDLK_w:
                    case SDLK_UP:
                        player.position.x += cos(deg_to_rad(player.angle)) * 15.0;
                        player.position.y += sin(deg_to_rad(player.angle)) * 15.0;
                        break;
                    case SDLK_s:
                    case SDLK_DOWN:
                        player.position.x -= cos(deg_to_rad(player.angle)) * 15.0;
                        player.position.y -= sin(deg_to_rad(player.angle)) * 15.0;
                        break;
                }
            } else if (event.type == SDL_MOUSEMOTION && (event.motion.state & SDL_BUTTON(SDL_BUTTON_LEFT))) {
                player.angle += event.motion.xrel * 0.3;
                if (player.angle < 0) player.angle += 360;
                if (player.angle >= 360) player.angle -= 360;
            }
        }
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); 
        SDL_RenderClear(renderer);

        for (int i = 0; i < WINDOW_WIDTH; i++) {
            double ray_angle = 
                deg_to_rad(player.angle) - 
                (deg_to_rad(FOV / 2)) + 
                ((double)i / (double)WINDOW_WIDTH) * 
                deg_to_rad(FOV);
            RayResult result = cast_ray_dda(&player, ray_angle);
            double corrected_dist = result.distance * cos(ray_angle - deg_to_rad(player.angle));
            int line_height = (int)(CELL_SIZE * WINDOW_HEIGHT / corrected_dist);

            Uint32 color = 0xFF646464; // Default color (Gray)
            switch(result.hit_wall) {
                case 1: color = 0xFFFF0000; break; // Red
                case 2: color = 0xFF00FF00; break; // Green
                case 3: color = 0xFF0000FF; break; // Blue
                default: color = 0xFF646464; break; // Gray
            }

            double shading = 1.0 - (corrected_dist / 2000.0); 

            int tex_x = (int)(result.wall_x * (double)TEX_WIDTH);

            if(result.side == 0 && cos(ray_angle) > 0) tex_x = TEX_WIDTH - tex_x - 1;
            if(result.side == 1 && sin(ray_angle) < 0) tex_x = TEX_WIDTH - tex_x - 1;


            int draw_start = (WINDOW_HEIGHT / 2) - (line_height / 2);
            int draw_end = (WINDOW_HEIGHT / 2) + (line_height / 2);
            if (draw_start < 0) draw_start = 0;
            if (draw_end >= WINDOW_HEIGHT) draw_end = WINDOW_HEIGHT - 1;

            double step = 1.0 * TEX_HEIGHT / line_height;
            double tex_pos = (draw_start - WINDOW_HEIGHT / 2 + line_height / 2) * step;

            // draw_line(renderer, i, draw_start, i, draw_end, color, shading);
            if (result.hit_wall == 1) {
                draw_line_textured(renderer, result, tex_x, step, i, draw_start, i, draw_end, texture_1, tex_pos, shading);
            } else if (result.hit_wall == 2) {
                draw_line_textured(renderer, result, tex_x, step, i, draw_start, i, draw_end, texture_2, tex_pos, shading);
            } else {
                draw_line_textured(renderer, result, tex_x, step, i, draw_start, i, draw_end, texture_3, tex_pos, shading);
            } 
        }
        // break;
        SDL_RenderPresent(renderer);
        SDL_Delay(delay);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}