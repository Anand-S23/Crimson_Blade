#ifndef CRIMSON_H
#define CRIMSON_H

typedef struct entity
{
    v2 position;
    v2 dimension;
    int grounded;
} entity; 

typedef struct game_state
{
    entity player; 
} game_state;

internal void UpdateApp(game_memory *memory, offscreen_buffer *buffer, game_input *input);

#endif