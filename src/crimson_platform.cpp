#include <SDL.h> 
#include <SDL_image.h>
#include <stdio.h> 

#include "crimson_platform.h"
#include "crimson.cpp"

global Platform GlobalPlatform; 

internal Window_Dimension GetWindowDimension(SDL_Window *window)
{
    Window_Dimension dimension; 
    SDL_GetWindowSize(window, &dimension.width, &dimension.height);
    return dimension; 
};

internal void ToggleFullscreen(SDL_Event *event, bool setFullscreen)
{
    SDL_Window *window = SDL_GetWindowFromID(event->key.windowID);

    if (setFullscreen)
    {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
    }
    else 
    {
        SDL_SetWindowFullscreen(window, 0);
    }
}

internal void UpdateWindow(SDL_Renderer *renderer)
{
    SDL_RenderPresent(renderer);
    SDL_Delay(1.0f / 60); // TODO: Change this to account for more or less time
};

internal void RenderClear(SDL_Renderer *renderer)
{
    SDL_SetRenderDrawColor(renderer, 52, 76, 76, 255);
    SDL_RenderClear(renderer);
}

internal void HandleEvent(SDL_Event *event)
{
    switch (event->type)
    {
        case SDL_QUIT:
        {
            GlobalPlatform.running = false;
        } break;

        case SDL_WINDOWEVENT:
        {
            switch (event->window.event)
            {
                case SDL_WINDOWEVENT_EXPOSED:
                {
                    SDL_Window *window = SDL_GetWindowFromID(event->window.windowID);
                    SDL_Renderer *renderer = SDL_GetRenderer(window);
                    UpdateWindow(renderer);
                };
            }
        }

        case SDL_KEYDOWN:
        case SDL_KEYUP:
        {
            SDL_Keycode keyCode = event->key.keysym.sym;
            bool isDown = (event->key.state == SDL_PRESSED);
            bool wasDown = false;

            if (event->key.state == SDL_RELEASED)
            {
                wasDown = true;
            }
            else if (event->key.repeat != 0)
            {
                wasDown = true;
            }
            
            if (event->key.repeat == 0)
            {
                int keyInput;

                // Getting input key
                {
                    if (keyCode >= SDLK_a && keyCode <= SDLK_z ||
                            keyCode >= SDLK_0 && keyCode <= SDLK_9)
                    {
                        keyInput = (keyCode >= SDLK_a && keyCode <= SDLK_z) ? 
                                    (int)KEY_a + (keyCode - SDLK_a) :
                                    (int)KEY_0 + (keyCode - SDLK_0);
                    }
                    else if (keyCode >= SDLK_F1 && keyCode <= SDLK_F12)
                    {
                        keyInput = (int)KEY_f1 + (keyCode - SDLK_F1);
                    }
                    else if (keyCode == SDLK_LALT)
                    {
                        keyInput = (int)KEY_alt;
                    }
                    else if (keyCode == SDLK_UP)
                    {
                        keyInput = (int)KEY_up;
                    }
                    else if (keyCode == SDLK_DOWN)
                    {
                        keyInput = (int)KEY_down;
                    }
                    else if (keyCode == SDLK_RIGHT)
                    {
                        keyInput = (int)KEY_right;
                    }
                    else if (keyCode == SDLK_LEFT)
                    {
                        keyInput = (int)KEY_left;
                    }
                    else if (keyCode == SDLK_BACKSPACE)
                    {
                        keyInput = (int)KEY_backspace;
                    }
                    else if (keyCode == SDLK_SPACE)
                    {
                        keyInput = (int)KEY_space; 
                    }
                    else if (keyCode == SDLK_TAB)
                    {
                        keyInput = (int)KEY_tab;
                    }
                    else if (keyCode == SDLK_LCTRL)
                    {
                        keyInput = (int)KEY_ctrl; 
                    }
                    else if (keyCode == SDLK_KP_ENTER)
                    {
                        keyInput = (int)KEY_enter;
                    }
                    else if (keyCode == SDLK_LSHIFT)
                    {
                        keyInput = (int)KEY_shift;
                    }
                }

                // Setting key state 
                {
                    if (isDown)
                    {
                        GlobalPlatform.keyPressed[keyInput] = KEY_pressed; 
                    }
                    if (wasDown)
                    {
                        GlobalPlatform.keyPressed[keyInput] = KEY_not_pressed;
                    }
                }

                // Checking global cases (quit | fullscreen)
                {
                    if (GlobalPlatform.keyPressed[KEY_alt] && 
                        GlobalPlatform.keyPressed[KEY_f4])
                    {
                        GlobalPlatform.running = false;
                    }
                    else if(GlobalPlatform.keyPressed[KEY_alt] &&
                            GlobalPlatform.keyPressed[KEY_f])
                    {
                        GlobalPlatform.fullscreen = !GlobalPlatform.fullscreen;
                        ToggleFullscreen(event, GlobalPlatform.fullscreen);
                    }
                }
            }
        }
    }
}

int main(int argc, char** argv)
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG);

    SDL_Window *window = SDL_CreateWindow("Crimson Blade", 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SDL_WINDOWPOS_UNDEFINED, 
                                          SCREEN_WIDTH, SCREEN_HEIGHT, 0);

    if (window)
    {
        SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

        if (renderer)
        {
            GlobalPlatform.prevTime = SDL_GetTicks();
            float lagTime = 0;

            while (GlobalPlatform.running)
            {
                GlobalPlatform.currentTime = SDL_GetTicks();
                GlobalPlatform.timeElpased = (float) (GlobalPlatform.currentTime - GlobalPlatform.prevTime) / 1000.0f;
                GlobalPlatform.prevTime = GlobalPlatform.currentTime;
                lagTime += GlobalPlatform.timeElpased;

                SDL_Event event; 
                while (SDL_PollEvent(&event))
                {
                    HandleEvent(&event);
                }

                // TODO: Fix refresh time
                while (lagTime >= GlobalPlatform.targetFPS)
                {
                    GameUpdateVideo(renderer, GlobalPlatform.keyPressed); 
                    lagTime -= GlobalPlatform.targetFPS; 
                }

                RenderClear(renderer);
                GameUpdateVideo(renderer, GlobalPlatform.keyPressed); 
                UpdateWindow(renderer);

            }
        }
        else 
        {
            // TODO: Logging
        }
    }
    else
    {
        // TODO: Logging 
    }

    IMG_Quit();
    SDL_Quit();
    return 0; 
}
