typedef struct color
{
    u8 r; 
    u8 g; 
    u8 b;
} crimson_color; 

internal crimson_color CreateColor(int r, int g, int b) 
{
    crimson_color color = { (u8)r, (u8)g, (u8)b };
    return color; 
}
#define C_Color(r, g, b) CreateColor(r, g, b)

internal void ClearBuffer(offscreen_buffer *buffer)
{
    int width = buffer->width;
    int height = buffer->height;
    
    u8 *row = (u8 *)buffer->memory;
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

        row += buffer->pitch;
    }
}

internal void DrawFilledRect(offscreen_buffer *buffer, 
                             int x, int y, int w, int h, 
                             crimson_color color)
{
    i32 min_x = (x < 0) ? 0 : (i32)x;
    i32 min_y = (y < 0) ? 0 : (i32)y;
    i32 max_x = (buffer->width < min_x + w) ? buffer->width : min_x + (i32)w;
    i32 max_y = (buffer->height < min_y + h) ? buffer->height : min_y + (i32)h;
    
    u8 *row = (u8 *)buffer->memory + x*buffer->bytes_per_pixel + y*buffer->pitch; 
    for(i32 j = min_y; j < max_y; ++j)
    {
        u32 *pixel = (u32 *)row;
        for(i32 i = min_x; i < max_x; ++i)
        {
            *pixel++ = ((color.r << 16) | (color.g << 8) | color.b);
        }

        row += buffer->pitch; 
    }
}
