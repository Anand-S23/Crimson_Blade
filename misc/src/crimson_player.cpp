#include "crimson_player.h"

#define PLAYER_MAX_VEL 0.05f

internal void PlayerInit(Player_Entity *player, v2 pos)
{
    player->pos = pos;
    player->initialized = true;
}

internal void PlayerUpdate(Player_Entity *player, bool input[])
{
    if (input[KEY_up] == KEY_pressed || input[KEY_w] == KEY_pressed &&
        player->grounded && !player->jumping)
    {
        player->vel.y -= PLAYER_MAX_VEL; 
        player->vel.y = 0;
        player->grounded = false; 
        player->jumping = true;
    }
    else 
    {
        player->vel.y = 0;
    }

    if (input[KEY_left] == KEY_pressed || input[KEY_d] == KEY_pressed)
    {
        player->vel.x -= PLAYER_MAX_VEL; 
    }
    else if (input[KEY_right] == KEY_pressed || input[KEY_a] == KEY_pressed)
    {
        player->vel.x += PLAYER_MAX_VEL;
    }
    else
    {
        player->vel.x = 0;
    }

    player->pos.x += player->vel.x;
    player->pos.y += player->vel.y + 1;
}
