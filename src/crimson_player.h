#pragma once 

enum Entity_Direction {
    DIRECTION_north, 
    DIRECTION_south,
    DIRECTION_east, 
    DIRECTION_west, 
    DIRECTION_north_east, 
    DIRECTION_north_west, 
    DIRECTION_south_east, 
    DIRECTION_south_west 
}

struct Entity {
    bool initialized = false; 
    v2 pos; 
    v2 vel; 
    Entity_Direction direction; 
};