#include "renderer.c"

typedef struct player
{
    int x; 
    int y; 
} player; 

internal void UpdateApp(offscreen_buffer *buffer)
{
    ClearBuffer(buffer);
    DrawFilledRect(buffer, 0, 600, 1280, 120, C_Color(100, 100, 100)); 


}
