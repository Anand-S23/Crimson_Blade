#define UI_MAX_WIDGETS 128
#define UI_MAX_AUTO_LAYOUT_GROUPS 16

#define UIIDGen() UIIDInit(__LINE__ + UI_SRC_ID, 0)
#define UIIDGenI(i) UIIDInit(__LINE__ + UI_SRC_ID, i)

enum
{
    UI_WIDGET_button,
    UI_WIDGET_slider,
};

typedef struct UIID
{
    u32 primary;
    u32 secondary;
}
UIID;

typedef struct UIWidget
{
    int type;
    UIID id;
    v4 rect;
    f32 t_hot;
    f32 t_active;
    
    union
    {
        struct Slider
        {
            f32 value;
        }
        slider;
    };
}
UIWidget;

typedef struct UIInput
{
    f32 cursor_x;
    f32 cursor_y;
    b32 left_cursor_down;
    b32 right_cursor_down;
}
UIInput;

typedef struct UI
{
    f32 cursor_x;
    f32 cursor_y;
    b32 left_cursor_down;
    b32 right_cursor_down;
    
    u32 widget_count;
    UIWidget widgets[UI_MAX_WIDGETS];
    
    u32 auto_layout_stack_pos;
    struct
    {
        b32 is_column;
        v2 position;
        v2 size;
        f32 progress;
    }
    auto_layout_stack[UI_MAX_AUTO_LAYOUT_GROUPS];
    
    UIID hot;
    UIID active;
}
UI;