#ifndef CRIMSON_PLATFORM_H
#define CRIMSON_PLATFORM_H

#if CRIMSON_SLOW
#define Assert(expr) if(!(expr)) {*(int *)0 = 0;}
#else 
#define Assert(expr)
#endif

#define Bytes(n)     (n)
#define Kilobytes(n) (Bytes(n)*1024)
#define Megabytes(n) (Kilobytes(n)*1024)
#define Gigabytes(n) (Megabytes(n)*1024)

#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))

// game buffer
typedef struct offscreen_buffer
{
    void *memory;
    int width; 
    int height; 
    int pitch; 
    int bytes_per_pixel;
} offscreen_buffer;

// input 
typedef struct game_button_state
{
    int half_transititions;
    b32 ended_down; 
} game_button_state; 

typedef struct game_controller_input
{
    b32 is_analog;

    f32 start_x; 
    f32 start_y;
    f32 end_x;
    f32 end_y;

    f32 min_x; 
    f32 min_y; 
    f32 max_x; 
    f32 max_y;

    union 
    {
        game_button_state buttons[6];

        struct 
        {
            game_button_state up; 
            game_button_state down; 
            game_button_state left; 
            game_button_state right; 
            game_button_state left_shoulder; 
            game_button_state right_shoulder; 
        };
    };
} game_controller_input;

typedef struct game_input
{
    game_controller_input controllers[4];
} game_input;

// game memory
typedef struct game_memory
{
    b32 initialized;
    u64 storage_size; 
    void *storage;
} game_memory;

#endif