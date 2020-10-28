#pragma once 

typedef struct Texture {
    unsigned char *pixel_data; 
    int width; 
    int height; 
    int channels;
} Texture;