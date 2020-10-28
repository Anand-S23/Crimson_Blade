#include "ui.h"
#include "renderer.h"

#define DEBUG_DRAW_collision_volumes (1<<0)
#define DEBUG_DRAW_hurt_volumes      (1<<1)

typedef struct App
{
    f32 delta_t;
    Texture image; 
    u32 debug_draw_flags;
} App;

global App *app = 0;
global Platform *platform = 0;