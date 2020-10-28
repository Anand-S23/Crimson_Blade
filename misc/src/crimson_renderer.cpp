#include "crimson_core.cpp" 

internal SDL_Texture *LoadTexture(SDL_Renderer *renderer, const char *filename)
{
    int width, height; 
    uint32_t *imagePixels = (uint32_t *) stbi_load(filename, &width, &height, NULL, 4); 

    SDL_Surface *imageSurface = SDL_CreateRGBSurfaceFrom(imagePixels, (int) width, (int) height, 
                                                         32, (int) width * 4, 0x000000FF,
                                                         0x0000FF00, 0x00FF0000, 0xFF000000);

    SDL_Texture *retTexture = SDL_CreateTextureFromSurface(renderer, imageSurface); 
    SDL_FreeSurface(imageSurface);
    return retTexture;
}

internal void BlitTextureFromFile(SDL_Renderer *renderer, const char *filename, v4 dst)
{
    SDL_Texture *texture = LoadTexture(renderer, filename); 
    SDL_Rect dstRect = GetRect(dst);
    SDL_RenderCopy(renderer, texture, NULL, &dstRect); 
}

internal void BlitTexture(SDL_Renderer *renderer, SDL_Texture *texture, v4 dst)
{
    SDL_Rect dstRect = GetRect(dst);
    SDL_RenderCopy(renderer, texture, NULL, &dstRect); 
}

internal void BlitTextureSlice(SDL_Renderer *renderer, SDL_Texture *texture, v4 src, v4 dst) 
{
    SDL_Rect srcRect = GetRect(src);
    SDL_Rect dstRect = GetRect(dst);
    SDL_RenderCopy(renderer, texture, &srcRect, &dstRect); 
}


internal void BlitTextureFromFileEx(SDL_Renderer *renderer, const char *filename) 
{
}

internal void BlitTextureEx(SDL_Renderer *renderer, SDL_Texture *texture) 
{
}

internal void RenderRect(SDL_Renderer *renderer, v2 pos, v2 dimension)
{
    SDL_SetRenderDrawColor(renderer, 100, 0, 0, 255);
    SDL_Rect rect = {(int)pos.x, (int)pos.y, (int)dimension.x, (int)dimension.y};
    SDL_RenderDrawRect(renderer, &rect);
    SDL_RenderFillRect(renderer, &rect);
}
