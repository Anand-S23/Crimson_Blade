#pragma once 

#define internal static 
#define global static 
#define local_persist static

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720

enum Key {
#define Key(name, str) KEY_##name,
#include "platform_key_list.h"
    KEY_MAX
};

enum Key_State {
    KEY_not_pressed,
    KEY_pressed
};

struct Window_Dimension {
    int width; 
    int height;
};

struct Platform {
    bool running = true;
    bool fullscreen = false; 

    // Time 
    int32_t timeElpased; 
    float currentTime;
    float prevTime;
    float targetFPS = 1.0f / 60;

    // Input
    bool keyPressed[KEY_MAX]; 
    float mouseX;
    float mouseY; 
    bool mouseRightDown; 
    bool mouseLeftDown;
}; 
