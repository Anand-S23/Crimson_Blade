#include "crimson.h"
#include "crimson_renderer.c"

internal void UpdateApp(game_memory *memory, offscreen_buffer *buffer, game_input *input)
{
    Assert(sizeof(game_state) <= memory->storage_size);

    game_state *state = (game_state *)memory->storage;
    if (!memory->initialized)
    {
        char *filename = __FILE__;

        debug_read_file_result file = DEBUGPlatformReadEntireFile(filename);
        if (file.memory)
        {
            DEBUGPlatformWriteFile("w:/crimson_blade/misc/test.out", file.size, file.memory);
            DEBUGPlatformFreeFileMemory(file.memory);
        }

        state->player.position = v2(10, 10);
        state->player.dimension = v2(20, 30);
        state->player.grounded = 0;

        memory->initialized = 1;
    }

    for (int controller_index = 0; controller_index < ArraySize(input->controllers); ++controller_index)
    {
        game_controller_input *controller = &input->controllers[controller_index];
        if (controller->is_analog)
        {
            state->player.position.x += controller->stick_average_x;
            state->player.position.y += controller->stick_average_y;
        }
        else
        {
            if (controller->right.ended_down)
            {
                state->player.position.x += 1;
            }
            if (controller->left.ended_down)
            {
                state->player.position.x -= 1;
            }
        }
    }

    ClearBuffer(buffer);

    //Texture img = LoadTexture("assests/platformer-tilemap.png");
    //BlitTextureToBuffer(buffer, img, v2(0, 0));

    DrawFilledRect(buffer, state->player.position, state->player.dimension, v3(0, 255, 255));
    DrawFilledRect(buffer, v2(0, 600), v2(1280, 120), v3(100, 100, 100)); 
}
