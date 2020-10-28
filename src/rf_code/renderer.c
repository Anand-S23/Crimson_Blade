#define STB_IMAGE_IMPLEMENTATION 
#include "stb_image.h"

#include "renderer.h"

#define RED_OFFSET   2
#define GREEN_OFFSET 1
#define BLUE_OFFSET  0

internal void
ClearBackbuffer(void)
{
    for(u32 j = 0; j < platform->backbuffer_height; ++j)
    {
        for(u32 i = 0; i < platform->backbuffer_width; ++i)
        {
            u8 *r = platform->backbuffer + (j*platform->backbuffer_width + i)*3 + RED_OFFSET;
            u8 *g = platform->backbuffer + (j*platform->backbuffer_width + i)*3 + GREEN_OFFSET;
            u8 *b = platform->backbuffer + (j*platform->backbuffer_width + i)*3 + BLUE_OFFSET;
            
            *r -= (u8)((f32)(*r) * 16.f * app->delta_t);
            *g -= (u8)((f32)(*g) * 16.f * app->delta_t);
            *b -= (u8)((f32)(*b) * 16.f * app->delta_t);
        }
    }
}

internal void
DrawFilledRect(v4 color, v2 position, v2 size)
{
    u8 *backbuffer = platform->backbuffer;
    i32 backbuffer_width = (i32)platform->backbuffer_width;
    i32 backbuffer_height = (i32)platform->backbuffer_height;
    
    i32 lower_bound_x = (i32)position.x;
    i32 lower_bound_y = (i32)position.y;
    i32 upper_bound_x = lower_bound_x + (i32)size.x;
    i32 upper_bound_y = lower_bound_y + (i32)size.y;
    i32 pixel_index = 0;
    
    for(i32 j = lower_bound_y; j <= upper_bound_y; ++j)
    {
        
        if(j >= 0 && j < backbuffer_height)
        {
            for(i32 i = lower_bound_x; i <= upper_bound_x; ++i)
            {
                
                if(i >= 0 && i < backbuffer_width)
                {
                    pixel_index = j*backbuffer_width + i;
                    backbuffer[pixel_index*3 + RED_OFFSET]   = (u8)(color.r * 255.f);
                    backbuffer[pixel_index*3 + GREEN_OFFSET] = (u8)(color.g * 255.f);
                    backbuffer[pixel_index*3 + BLUE_OFFSET]  = (u8)(color.b * 255.f);
                }
            }
        }
    }
}

internal void
DrawFilledCircle(v4 color, v2 position, f32 radius)
{
    u8 *backbuffer = platform->backbuffer;
    i32 backbuffer_width = (i32)platform->backbuffer_width;
    i32 backbuffer_height = (i32)platform->backbuffer_height;
    
    i32 lower_bound_x = (i32)position.x - (i32)radius;
    i32 lower_bound_y = (i32)position.y - (i32)radius;
    i32 upper_bound_x = (i32)position.x + (i32)radius;
    i32 upper_bound_y = (i32)position.y + (i32)radius;
    i32 pixel_index = 0;
    
    f32 radius_sq = radius*radius;
    
    for(i32 j = lower_bound_y; j <= upper_bound_y; ++j)
    {
        
        if(j >= 0 && j < backbuffer_height)
        {
            for(i32 i = lower_bound_x; i <= upper_bound_x; ++i)
            {
                
                if(i >= 0 && i < backbuffer_width)
                {
                    
                    v2 pixel_position = {
                        (f32)i,
                        (f32)j,
                    };
                    
                    f32 distance_sq =
                        (position.x - pixel_position.x)*(position.x - pixel_position.x) +
                        (position.y - pixel_position.y)*(position.y - pixel_position.y);
                    
                    if(distance_sq <= radius_sq)
                    {
                        pixel_index = j*backbuffer_width + i;
                        backbuffer[pixel_index*3 + RED_OFFSET]   = (u8)(color.r * 255.f);
                        backbuffer[pixel_index*3 + GREEN_OFFSET] = (u8)(color.g * 255.f);
                        backbuffer[pixel_index*3 + BLUE_OFFSET]  = (u8)(color.b * 255.f);
                    }
                }
            }
        }
    }
}

internal Texture LoadTexture(char *filename)
{
    int width, height, channels;

    Texture texture = {0};
    texture.pixel_data = stbi_load(filename, &width, &height, &channels, STBI_rgb);
    texture.width = width;
    texture.height = height;
    texture.channels = channels;

    return texture; 
}

internal void BlitImage(Texture image, v2 position)
{
    u8 *backbuffer = platform->backbuffer;
    i32 backbuffer_width = (i32)platform->backbuffer_width;
    i32 backbuffer_height = (i32)platform->backbuffer_height;
    
    i32 lower_bound_x = (i32)position.x;
    i32 lower_bound_y = (i32)position.y;
    i32 upper_bound_x = lower_bound_x + (i32)image.width;
    i32 upper_bound_y = lower_bound_y + (i32)image.height;
    i32 pixel_index = 0;

    for(i32 j = lower_bound_y; j <= upper_bound_y; ++j)
    {
        
        if(j >= 0 && j < backbuffer_height)
        {
            for(i32 i = lower_bound_x; i <= upper_bound_x; ++i)
            {
                
                if(i >= 0 && i < backbuffer_width)
                {
                    unsigned char *pixel_offset = image.pixel_data + (i + image.width * j) * image.channels;
                    unsigned char r = pixel_offset[0];
                    unsigned char g = pixel_offset[1];
                    unsigned char b = pixel_offset[2];
    
                    pixel_index = j*backbuffer_width + i;
                    backbuffer[pixel_index*3 + RED_OFFSET]   = (u8)r;
                    backbuffer[pixel_index*3 + GREEN_OFFSET] = (u8)g;
                    backbuffer[pixel_index*3 + BLUE_OFFSET]  = (u8)b;
                }
            }
        }
    }
}