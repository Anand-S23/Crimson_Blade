#ifndef CRIMSON_RENDERER_H
#define CRIMSON_RENDERER_H

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

typedef struct Texture
{
    unsigned char *data; 
    int width; 
    int height; 
    int channels;
} Texture; 

#endif