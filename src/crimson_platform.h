#define ArrayCount(arr) (sizeof(arr) / sizeof((arr)[0]))

typedef struct offscreen_buffer
{
    void *memory;
    int width; 
    int height; 
    int pitch; 
    int bytes_per_pixel;
} offscreen_buffer;

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
