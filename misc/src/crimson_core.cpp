internal SDL_Rect GetRect(v4 rect) 
{
    SDL_Rect retRect = { (int) rect.x, (int) rect.y, (int) rect.width, (int) rect.height }; 
    return retRect;
}
