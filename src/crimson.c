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

internal void UpdateApp(Platform *platform)
{
    ClearBuffer(platform);

    if (!Player.intialized)
    {
        Player.x = 10; 
        Player.y = 10, 
        Player.width = 20; 
        Player.height = 30; 
        Player.grounded = 0; 
        Player.intialized = 1;
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

    DrawFilledRect(platform, Player.x, Player.y, Player.width, Player.height, C_Color(0, 255, 255));
    DrawFilledRect(platform, 0, 600, 1280, 120, C_Color(100, 100, 100)); 


}
