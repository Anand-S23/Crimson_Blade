#include "crimson.h"
#include "crimson_math.h"
#include "crimson_entity.h"
#include "crimson_player.cpp"
#include "crimson_renderer.cpp"

global Player_Entity player;

internal void GameUpdateVideo(SDL_Renderer *renderer, Platform *platform)
{

    if (!player.initialized)
    {
        PlayerInit(&player, v2(100, 100));
    }

    PlayerUpdate(&player, platform->keyPressed);
    RenderRect(renderer, player.pos, v2(50, 50));

    local_persist SDL_Texture *tilemap = LoadTexture(renderer, "assests/platformer-tilemap.png");

//    BlitTexture(renderer, tilemap, v4(10, 10, 700, 500));
    BlitTextureSlice(renderer, tilemap, v4(0, 0, 50, 50), v4(10, 10, 700, 500));
}
