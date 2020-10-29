#include <Windows.h> 
#include <stdint.h>

#include "platform.h"
#include "app.c"

global b32 Running = 1; 
global win32_offscreen_buffer Global_Backbuffer; 

window_dimension GetWindowDimension(HWND window)
{
    window_dimension result;

    RECT client_rect;
    GetClientRect(window, &client_rect);
    result.width = client_rect.right - client_rect.left;
    result.height = client_rect.bottom - client_rect.top;

    return result;
}

internal void Win32ResizeDIBSection(win32_offscreen_buffer *buffer, 
                                    int width, int height)
{
    if (buffer->memory)
    {
        VirtualFree(buffer->memory, 0, MEM_FREE);
    }

    buffer->width = width; 
    buffer->height = height; 
    buffer->bytes_per_pixel = 4;

    buffer->info.bmiHeader.biSize = sizeof(buffer->info.bmiHeader);
    buffer->info.bmiHeader.biWidth = buffer->width;
    buffer->info.bmiHeader.biHeight = -buffer->height;
    buffer->info.bmiHeader.biPlanes = 1;
    buffer->info.bmiHeader.biBitCount = 32;
    buffer->info.bmiHeader.biCompression = BI_RGB;

    int memory_size = buffer->width * buffer->height * buffer->bytes_per_pixel;
    buffer->memory = VirtualAlloc(0, memory_size, MEM_COMMIT, PAGE_READWRITE);
    buffer->pitch = buffer->width * buffer->bytes_per_pixel;
}

internal void Win32DisplayBufferInWindow(HDC device_context, 
                                         int window_width, int window_height, 
                                         win32_offscreen_buffer *buffer)
{
    StretchDIBits(device_context, 
                 0, 0, window_width, window_height,
                 0, 0, buffer->width, buffer->height, 
                 buffer->memory, &buffer->info, 
                 DIB_RGB_COLORS, SRCCOPY);
}

LRESULT CALLBACK Win32WindowProc(HWND window, UINT message, 
                                 WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0; 

    switch (message)
    {
        case WM_CLOSE: 
        case WM_DESTROY: 
        {
            Running = 0; 
        } break;

        case WM_SIZE: 
        {
        } break; 

        case WM_SYSKEYDOWN:
        case WM_SYSKEYUP:
        case WM_KEYDOWN:
        case WM_KEYUP:
        {
            uint32_t vk_code = w_param;
            b32 was_down = (l_param & (1 << 30) != 0);
            b32 is_down = (l_param & (1 << 31) == 0);

            if (is_down != was_down)
            {
                if (vk_code == 'W')
                {
                }
                else if (vk_code == 'A')
                {
                }
                else if (vk_code == 'S')
                {
                }
                else if (vk_code == 'D')
                {
                }
                else if (vk_code == 'Q')
                {
                }
                else if (vk_code == 'E')
                {
                }
                else if (vk_code == VK_UP)
                {
                }
                else if (vk_code == VK_DOWN)
                {
                }
                else if (vk_code == VK_LEFT)
                {
                }
                else if (vk_code == VK_RIGHT)
                {
                }
                else if (vk_code == VK_SPACE)
                {
                }
                else if (vk_code == VK_ESCAPE)
                {
                }
            }

            b32 alt_key_was_down = ((l_param & (1 << 29)) != 0);
            if ((vk_code == VK_F4) && alt_key_was_down)
            {
                Running = 0; 
            }
        } break; 

        case WM_PAINT: 
        {
            window_dimension dimension = GetWindowDimension(window);

            PAINTSTRUCT paint; 
            HDC device_context = BeginPaint(window, &paint);
            Win32DisplayBufferInWindow(device_context, 
                                       dimension.width, dimension.height, 
                                       &Global_Backbuffer);
            EndPaint(window, &paint);
        } break;

        default: 
        {
            result = DefWindowProcA(window, message, w_param, l_param);
        } break;
    }

    return result; 
}

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, 
                   LPSTR cmd_line, int cmd_show)
{
    Win32ResizeDIBSection(&Global_Backbuffer, 1280, 720);
    WNDCLASSA window_class = {0};

    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "CrimsonBladeWindowClass";
    
    if (RegisterClassA(&window_class))
    {
        HWND window = CreateWindowExA(0, window_class.lpszClassName, "Crimson Blade", 
                                      WS_OVERLAPPEDWINDOW | WS_VISIBLE, 
                                      CW_USEDEFAULT, CW_USEDEFAULT, 
                                      1280, 720, 0, 0, instance, 0); 

        if (window)
        {
            while (Running)
            {
                MSG Message; 
                while (PeekMessageA(&Message, 0, 0, 0, PM_REMOVE))
                {
                    if (Message.message == WM_QUIT)
                    {
                        Running = 0;
                    }

                    TranslateMessage(&Message);
                    DispatchMessageA(&Message); 
                }

                offscreen_buffer buffer = {0};
                {
                    buffer.memory = Global_Backbuffer.memory;
                    buffer.width = Global_Backbuffer.width; 
                    buffer.height = Global_Backbuffer.height; 
                    buffer.pitch = Global_Backbuffer.pitch; 
                    buffer.bytes_per_pixel = Global_Backbuffer.bytes_per_pixel;
                }

                UpdateApp(&buffer);

                HDC device_context = GetDC(window);
                window_dimension dimension = GetWindowDimension(window);
                Win32DisplayBufferInWindow(device_context,
                                           dimension.width, dimension.height,
                                           &Global_Backbuffer);
                ReleaseDC(window, device_context);
            }
        }
        else
        {
            // TODO: Logging
        }
    }
    else 
    {
        // TODO: Logging
    }

    return 0; 
}
