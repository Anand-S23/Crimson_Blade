internal UIID
UIIDInit(u32 primary, u32 secondary)
{
    UIID id = { primary, secondary };
    return id;
}

internal UIID
UIIDNull(void)
{
    UIID id = { 0, 0 };
    return id;
}

internal b32
UIIDEqual(UIID id1, UIID id2)
{
    return (id1.primary == id2.primary &&
            id1.secondary == id2.secondary);
}

internal v4
UIGetNextAutoLayoutRect(UI *ui)
{
    v4 rect = {0};
    
    if(ui->auto_layout_stack_pos > 0)
    {
        u32 i = ui->auto_layout_stack_pos-1;
        
        rect.x = ui->auto_layout_stack[i].position.x;
        rect.y = ui->auto_layout_stack[i].position.y;
        rect.width = ui->auto_layout_stack[i].size.x;
        rect.height = ui->auto_layout_stack[i].size.y;
        
        if(ui->auto_layout_stack[i].is_column)
        {
            rect.y += ui->auto_layout_stack[i].progress;
            ui->auto_layout_stack[i].progress += rect.height;
        }
        else
        {
            rect.x += ui->auto_layout_stack[i].progress;
            ui->auto_layout_stack[i].progress += rect.width;
        }
    }
    else
    {
        SoftAssert("Auto-layout attempted without proper auto-layout group." == 0);
        rect = v4(0, 0, 64, 64);
    }
    
    return rect;
}

internal void
UIBeginFrame(UI *ui, UIInput *input)
{
    ui->cursor_x = input->cursor_x;
    ui->cursor_y = input->cursor_y;
    ui->left_cursor_down = input->left_cursor_down;
    ui->right_cursor_down = input->right_cursor_down;
    
    ui->widget_count = 0;
}

internal void
UIEndFrame(UI *ui)
{
    for(u32 i = 0; i < ui->widget_count; ++i)
    {
        UIWidget *widget = ui->widgets + i;
        
        widget->t_hot += ((f32)(!!UIIDEqual(ui->hot, widget->id)) - widget->t_hot) * app->delta_t * 4.f;
        widget->t_active += ((f32)(!!UIIDEqual(ui->active, widget->id)) - widget->t_active) * app->delta_t * 4.f;
        
        switch(widget->type)
        {
            
            case UI_WIDGET_button:
            {
                v4 color = {
                    0.6f + widget->t_hot * 0.4f - widget->t_active * 0.5f,
                    0.6f + widget->t_hot * 0.4f - widget->t_active * 0.5f,
                    0.6f + widget->t_hot * 0.4f - widget->t_active * 0.5f,
                    0.6f + widget->t_hot * 0.4f - widget->t_active * 0.5f,
                };
                
                DrawFilledRect(color,
                               v2(widget->rect.x, widget->rect.y),
                               v2(widget->rect.width, widget->rect.height));
                break;
            }
            
            case UI_WIDGET_slider:
            {
                DrawFilledRect(v4(0.6f, 0.6f, 0.6f, 0.6f),
                               v2(widget->rect.x, widget->rect.y),
                               v2(widget->rect.width, widget->rect.height));
                
                DrawFilledRect(v4(0.8f, 0.8f, 0.8f, 0.8f),
                               v2(widget->rect.x, widget->rect.y),
                               v2(widget->rect.width * widget->slider.value,
                                  widget->rect.height));
                
                break;
            }
            
            default: INVALID_CODE_PATH; break;
        }
    }
}

internal void
UIPushColumn(UI *ui, v2 position, v2 size)
{
    Assert(ui->auto_layout_stack_pos < UI_MAX_AUTO_LAYOUT_GROUPS);
    u32 i = ui->auto_layout_stack_pos++;
    ui->auto_layout_stack[i].is_column = 1;
    ui->auto_layout_stack[i].position = position;
    ui->auto_layout_stack[i].size = size;
    ui->auto_layout_stack[i].progress = 0;
}

internal void
UIPopColumn(UI *ui)
{
    if(ui->auto_layout_stack_pos > 0)
    {
        --ui->auto_layout_stack_pos;
    }
}

internal b32
UIButtonP(UI *ui, UIID id, char *text, v4 rect)
{
    Assert(ui->widget_count < UI_MAX_WIDGETS);
    
    b32 is_triggered = 0;
    
    b32 cursor_is_over = (ui->cursor_x >= rect.x &&
                          ui->cursor_x <= rect.x + rect.width &&
                          ui->cursor_y >= rect.y &&
                          ui->cursor_y <= rect.y + rect.height);
    
    if(!UIIDEqual(ui->hot, id) && cursor_is_over)
    {
        ui->hot = id;
    }
    else if(UIIDEqual(ui->hot, id) && !cursor_is_over)
    {
        ui->hot = UIIDNull();
    }
    
    if(UIIDEqual(ui->active, id))
    {
        if(!ui->left_cursor_down)
        {
            is_triggered = UIIDEqual(ui->hot, id);
            ui->active = UIIDNull();
        }
    }
    else
    {
        if(UIIDEqual(ui->hot, id))
        {
            if(ui->left_cursor_down)
            {
                ui->active = id;
            }
        }
    }
    
    UIWidget *widget = ui->widgets + ui->widget_count++;
    widget->type = UI_WIDGET_button;
    widget->id = id;
    widget->rect = rect;
    
    return is_triggered;
}

internal b32
UIButton(UI *ui, UIID id, char *text)
{
    v4 rect = UIGetNextAutoLayoutRect(ui);
    return UIButtonP(ui, id, text, rect);
}

internal f32
UISliderP(UI *ui, UIID id, char *text, f32 value, v4 rect)
{
    Assert(ui->widget_count < UI_MAX_WIDGETS);
    
    b32 cursor_is_over = (ui->cursor_x >= rect.x &&
                          ui->cursor_x <= rect.x + rect.width &&
                          ui->cursor_y >= rect.y &&
                          ui->cursor_y <= rect.y + rect.height);
    
    if(!UIIDEqual(ui->hot, id) && cursor_is_over)
    {
        ui->hot = id;
    }
    else if(UIIDEqual(ui->hot, id) && !cursor_is_over)
    {
        ui->hot = UIIDNull();
    }
    
    if(!UIIDEqual(ui->active, id))
    {
        if(UIIDEqual(ui->hot, id))
        {
            if(ui->left_cursor_down)
            {
                ui->active = id;
            }
        }
    }
    
    if(UIIDEqual(ui->active, id))
    {
        if(ui->left_cursor_down)
        {
            value = (ui->cursor_x - rect.x) / rect.width;
        }
        else
        {
            ui->active = UIIDNull();
        }
    }
    
    if(value < 0.f)
    {
        value = 0.f;
    }
    else if(value > 1.f)
    {
        value = 1.f;
    }
    
    UIWidget *widget = ui->widgets + ui->widget_count++;
    widget->type = UI_WIDGET_slider;
    widget->id = id;
    widget->rect = rect;
    widget->slider.value = value;
    
    return value;
}

internal f32
UISlider(UI *ui, UIID id, char *text, f32 value)
{
    v4 rect = UIGetNextAutoLayoutRect(ui);
    return UISliderP(ui, id, text, value, rect);
}