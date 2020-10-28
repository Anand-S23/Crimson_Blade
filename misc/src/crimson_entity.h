#pragma once 

enum Entity_Direction {
    DIRECTION_left,
    DIRECTION_right
};

struct Player_Entity
{
    bool initialized = false; 
    v2 pos; 
    v2 vel; 
    bool jumping = false; 
    bool grounded = true; 
    Entity_Direction direction; 
};
