#include "crimson_renderer.c"

typedef struct entity
{
    int x; 
    int y;
    int width; 
    int height; 

    int grounded;
    b32 intialized; 
} entity; 

global entity Player; 

internal void UpdateApp(offscreen_buffer *buffer, game_input *input)
{
    ClearBuffer(buffer);

    if (!Player.intialized)
    {
        Player.x = 10; 
        Player.y = 10, 
        Player.width = 20; 
        Player.height = 30; 
        Player.grounded = 0; 
        Player.intialized = 1;
    }

    game_controller_input *input_0 = &input->controllers[0];
    if (input_0->right.ended_down)
    {
        Player.y = 100;
        Player.x++;
    }

    if (!Player.grounded)
    {
        Player.y += 1; 
    }

    if (Player.y + Player.height <= 600)
    {
        Player.y = 600 - Player.height; 
        Player.grounded = 1;
    }

    DrawFilledRect(buffer, Player.x, Player.y, Player.width, Player.height, C_Color(0, 255, 255));
    DrawFilledRect(buffer, 0, 600, 1280, 120, C_Color(100, 100, 100)); 


}
