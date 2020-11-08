#include "crimson.h"
#include "crimson_renderer.c"

internal void UpdateApp(game_memory *memory, offscreen_buffer *buffer, game_input *input)
{
    Assert(sizeof(game_state) <= memory->storage_size);

    game_state *state = (game_state *)memory->storage;
    if (!memory->initialized)
    {
        state->player.position = v2(10, 10);
        state->player.dimension = v2(20, 30);
        state->player.grounded = 0;

        memory->initialized = 1;
    }

    game_controller_input *input_0 = &input->controllers[0];
    if (input_0->right.ended_down)
    {
        state->player.position.y = 100;
        state->player.position.x++;
    }

    if (!state->player.grounded)
    {
        state->player.position.y += 1; 
    }

    if (state->player.position.y + state->player.dimension.height <= 600)
    {
        state->player.position.y = 600 - state->player.dimension.height; 
        state->player.grounded = 1;
    }

    ClearBuffer(buffer);

    Texture img = LoadTexture("assests/platformer-tilemap.png");
    BlitTextureToBuffer(buffer, img, v2(0, 0));

    DrawFilledRect(buffer, state->player.position, state->player.dimension, v3(0, 255, 255));
    DrawFilledRect(buffer, v2(0, 600), v2(1280, 120), v3(100, 100, 100)); 
}
