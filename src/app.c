#include "renderer.c"

internal void UpdateApp(offscreen_buffer *buffer)
{
    ClearBuffer(buffer);
    DrawFilledRect(buffer, 0, 600, 1280, 120, C_Color(100, 100, 100)); 
    printf("%d ", buffer->height);
}
