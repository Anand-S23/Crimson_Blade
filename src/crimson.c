#include "crimson.h"
#include "crimson_renderer.c"

internal void UpdateApp(game_memory *memory, offscreen_buffer *buffer, game_input *input)
{
    Assert(sizeof(game_state) <= memory->storage_size);

    game_state *state = (game_state *)memory->storage;
    if (!memory->initialized)
    {
        state->player.x = 10;
        state->player.y = 10;
        state->player.width = 20;
        state->player.height = 30;
        state->player.grounded = 0;

        memory->initialized = 1;
    }

    game_controller_input *input_0 = &input->controllers[0];
    if (input_0->right.ended_down)
    {
        state->player.y = 100;
        state->player.x++;
    }

    if (!state->player.grounded)
    {
        state->player.y += 1; 
    }

    if (state->player.y + state->player.height <= 600)
    {
        state->player.y = 600 - state->player.height; 
        state->player.grounded = 1;
    }

    ClearBuffer(buffer);

    Texture img = LoadTexture("assests/platformer-tilemap.png");
    BlitTextureToBuffer(buffer, img, 0, 0);

    DrawFilledRect(buffer, state->player.x, state->player.y, 
                   state->player.width, state->player.height, C_Color(0, 255, 255));
    DrawFilledRect(buffer, 0, 600, 1280, 120, C_Color(100, 100, 100)); 
}
