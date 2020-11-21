#ifndef CRIMSON_PLATFORM_H
#define CRIMSON_PLATFORM_H

#if CRIMSON_SLOW
#define Assert(expr) if(!(expr)) {*(int *)0 = 0;}
#else 
#define Assert(expr)
#endif

inline u32 SafeTruncateUInt64(u64 value)
{
    Assert(value <= 0xFFFFFFFF);
    return (u32)value;
}

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
enum {
#define Key(name, str) KEY_##name,
#include "platform_key_list.h"
    KEY_MAX
};

typedef struct game_button_state
{
    int half_transititions;
    b32 ended_down; 
} game_button_state; 

typedef struct game_controller_input
{
    b32 is_analog;
    b32 is_connected;
    f32 stick_average_x; 
    f32 stick_average_y;

    union 
    {
        game_button_state buttons[12];

        struct 
        {
            // controller
            game_button_state up;
            game_button_state down;
            game_button_state left;
            game_button_state right;

            game_button_state left_shoulder; 
            game_button_state right_shoulder;

            game_button_state a_button;
            game_button_state x_button;
            game_button_state y_button;
            game_button_state b_button;

            game_button_state start;
            game_button_state back;

            // keyboard 
            //game_button_state keyboard[KEY_MAX];
        };
    };
} game_controller_input;

typedef struct game_input
{
    game_controller_input controllers[5]; 
} game_input;

inline game_controller_input *GetController(game_input *input, int unsigned controller_index) 
{ 
    Assert(controller_index < ArraySize(input->controllers)); 
    return &input->controllers[controller_index]; 
}

// game memory
typedef struct game_memory
{
    b32 initialized;
    u64 storage_size; 
    void *storage;
    u64 transient_storage_size; 
    void *transient_storage;
} game_memory;

typedef struct game_sound_output_buffer
{
    int samples_per_second;
    int sample_count;
    i16 *samples;
} game_sound_output_buffer;


#ifdef CRIMSON_INTERNAL
typedef struct debug_read_file_result
{
    void *memory;
    u32 size;
} debug_read_file_result;

internal debug_read_file_result DEBUGPlatformReadEntireFile(char *filename);
internal void DEBUGPlatformFreeFileMemory(void *memory);
internal b32 DEBUGPlatformWriteFile(char *filename, u32 memory_size, void *memory);
#endif

#endif