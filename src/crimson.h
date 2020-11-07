#ifndef CRIMSON_H
#define CRIMSON_H

typedef struct entity
{
    int x; 
    int y;
    int width; 
    int height; 
    int grounded;
} entity; 

typedef struct game_state
{
    entity player; 
} game_state;


#endif