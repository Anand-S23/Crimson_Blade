#pragma once 

struct Font_Dimension {
    int rowSize; 
    int charWidth; 
    int charHeight; 
}

struct Font {
    SDL_Texture *source; 
    Font_Dimension fontSize; 
}
