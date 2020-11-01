#include "crimson_renderer.h"

internal void ClearBuffer(Platform *platform)
{
    int width = platform->width;
    int height = platform->height;
    
    u8 *row = (u8 *)platform->memory;
    for (int y = 0; y < height; ++y)
    {
        u32 *pixel = (u32 *)row;
        for (int x = 0; x < width; ++x)
        {
            u8 blue = (u8)0;
            u8 green = (u8)0;
            u8 red = (u8)0;

            *pixel++ = ((red << 16) | (green << 8) | blue);
        }

        row += platform->pitch;
    }
}

internal void DrawFilledRect(Platform *platform, 
                             int x, int y, int w, int h, 
                             crimson_color color)
{
    i32 min_x = (x < 0) ? 0 : (i32)x;
    i32 min_y = (y < 0) ? 0 : (i32)y;
    i32 max_x = (platform->width < min_x + w) ? platform->width : min_x + (i32)w;
    i32 max_y = (platform->height < min_y + h) ? platform->height : min_y + (i32)h;
    
    u8 *row = (u8 *)platform->memory + x*platform->bytes_per_pixel + y*platform->pitch; 
    for(i32 j = min_y; j < max_y; ++j)
    {
        u32 *pixel = (u32 *)row;
        for(i32 i = min_x; i < max_x; ++i)
        {
            *pixel++ = ((color.r << 16) | (color.g << 8) | color.b);
        }

        row += platform->pitch; 
    }
}
