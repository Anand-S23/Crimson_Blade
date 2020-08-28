#include <SDL.h>

#include "crimson.h"
#include "crimson_math.h"
#include "crimson_player.h"

#define PLAYER_DEF_VEL 0.35f

internal void RenderRect(SDL_Renderer *renderer, int x, int y, int width, int height)
{
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_Rect rect = {x, y, width, height};
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
}

internal void GameUpdateVideo(SDL_Renderer *renderer, bool input[])
{
    static Entity player;

    if (!player.initialized)
    {
        player.pos = { 100, 100 };
        player.initialized = true;
    }

    if (input[KEY_up] == KEY_pressed || input[KEY_w] == KEY_pressed &&
        input[KEY_right] == KEY_pressed || input[KEY_d] == KEY_pressed)
    {
        // set vel correct
        player.direction = DIRECTION_north_east;
    }
    else if (input[KEY_up] == KEY_pressed || input[KEY_w] == KEY_pressed &&
             input[KEY_left] == KEY_pressed || input[KEY_a] == KEY_pressed)
    {
        // set vel correct
        player.direction = DIRECTION_north_west;
    }
    else if (input[KEY_down] == KEY_pressed || input[KEY_s] == KEY_pressed &&
             input[KEY_left] == KEY_pressed || input[KEY_a] == KEY_pressed)
    {
        // set vel correct
        player.direction = DIRECTION_south_west;
    }
    else if (input[KEY_down] == KEY_pressed || input[KEY_s] == KEY_pressed &&
             input[KEY_right] == KEY_pressed || input[KEY_d] == KEY_pressed)
    {
        // set vel correct
        player.direction = DIRECTION_south_east;
    }
    else if (input[KEY_up] == KEY_pressed || input[KEY_w] == KEY_pressed)
    {
        player.direction = DIRECTION_north;
    }
    else if (input[KEY_down] == KEY_pressed || input[KEY_s] == KEY_pressed)
    {
        player.direction = DIRECTION_south;
    }
    else if (input[KEY_left] == KEY_pressed || input[KEY_d] == KEY_pressed)
    {
        player.direction = DIRECTION_west;
    }
    else if (input[KEY_right] == KEY_pressed || input[KEY_a] == KEY_pressed)
    {
        player.direction = DIRECTION_east;
    }
    else
    {
        player.vel = {0, 0};
    }


    player.pos.x += player.vel.x;
    player.pos.y += player.vel.y;

    RenderRect(renderer, player.pos.x, player.pos.y, 50, 50);
}
