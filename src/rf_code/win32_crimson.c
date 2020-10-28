#include <windows.h>
#include "program_options.h"
#include "language_layer.h"
#include "platform.h"
#include "app.c"

global Platform global_platform = {0};

LRESULT CALLBACK Win32WindowProc(HWND window, UINT message, 
                                 WPARAM w_param, LPARAM l_param)
{
    LRESULT result = 0;

    switch (message)
    {
        case WM_DESTROY: 
        case WM_QUIT:
        {
            global_platform.quit = 1;
        } break; 

        case WM_KEYDOWN: 
        case WM_KEYUP: 
        {
            // TODO: keyboard input overhaul
            b32 key_is_down = message == WM_KEYDOWN;
            u32 key_code = w_param;
            u32 key_index = 0;
            if(key_code >= 'A' && key_code <= 'Z')
            {
                key_index = KEY_a + (key_code - 'A');
            }
            else if (key_code >= '0' && key_code <= '9')
            {
                key_index = KEY_0 + (key_code - '0');
            }
            global_platform.key_down[key_index] = key_is_down;
        } break; 

        case WM_MOUSEMOVE: 
        {
            global_platform.mouse_x = (f32)(l_param & 0x0000FFFF);
            global_platform.mouse_y = (f32)((l_param & 0xFFFF0000) >> 16);
        } break; 

        case WM_LBUTTONDOWN: 
        {
            if(!global_platform.left_mouse_down)
            {
                global_platform.left_mouse_pressed = 1;
            }

            global_platform.left_mouse_down = 1;
        } break; 

        case WM_LBUTTONUP:
        {
            global_platform.left_mouse_down = 0;
        } break; 

        case WM_RBUTTONDOWN: 
        {
            if(!global_platform.right_mouse_down)
            {
                global_platform.right_mouse_pressed = 1;
            }
            
            global_platform.right_mouse_down = 1;
        } break;

        case WM_RBUTTONUP:
        {
            global_platform.right_mouse_down = 0;
        } break; 

        default: 
        {
            result = DefWindowProc(window, message, w_param, l_param);
        } break;
    }
    
    return result;
}

int CALLBACK WinMain(HINSTANCE instance, HINSTANCE previousInstance, 
                     LPSTR commandLine, int commandShow)
{
    // Create the window class.
    WNDCLASSA window_class = {0};
    window_class.style = CS_HREDRAW | CS_VREDRAW;
    window_class.lpfnWndProc = Win32WindowProc;
    window_class.hInstance = instance;
    window_class.lpszClassName = "handmade_network_game";
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH) (COLOR_WINDOW + 1);
    RegisterClassA(&window_class);
    
    // Create and show the actual window.
    HWND window = CreateWindowA(window_class.lpszClassName,
                                WINDOW_TITLE,
                                WS_OVERLAPPEDWINDOW,
                                CW_USEDEFAULT, CW_USEDEFAULT,
                                DEFAULT_WINDOW_WIDTH, DEFAULT_WINDOW_HEIGHT,
                                0, 0,
                                instance, 0);
    
    if(!window)
    {
        DWORD error = GetLastError();
        fprintf(stderr, "ERROR: Window creation failed (%i).\n", error);
        goto quit;
    }
    
    HDC window_device_context = GetDC(window);
    
    ShowWindow(window, commandShow);
    UpdateWindow(window);
    
    struct {
        u32 width;
        u32 height;
        u8 *memory;
        struct {
            BITMAPINFOHEADER bmiHeader;
            void *bmi_colors_pointer;
        } bitmap_info;
    } backbuffer = {0};
    
    // NOTE: Backbuffer initialization
    {
        backbuffer.width = DEFAULT_WINDOW_WIDTH;
        backbuffer.height = DEFAULT_WINDOW_HEIGHT;
        u32 size_of_backbuffer = backbuffer.width * backbuffer.height * 3;
        backbuffer.memory = VirtualAlloc(0, size_of_backbuffer,
                                         MEM_COMMIT | MEM_RESERVE,
                                         PAGE_READWRITE);
        
        backbuffer.bitmap_info.bmiHeader.biSize = sizeof(backbuffer.bitmap_info);
        backbuffer.bitmap_info.bmiHeader.biWidth = backbuffer.width;
        backbuffer.bitmap_info.bmiHeader.biHeight = -backbuffer.height;
        backbuffer.bitmap_info.bmiHeader.biPlanes = 1;
        backbuffer.bitmap_info.bmiHeader.biBitCount = 24;
        backbuffer.bitmap_info.bmiHeader.biCompression = BI_RGB;
        backbuffer.bitmap_info.bmiHeader.biSizeImage = 0;
        backbuffer.bitmap_info.bmiHeader.biXPelsPerMeter = 0;
        backbuffer.bitmap_info.bmiHeader.biYPelsPerMeter = 0;
        backbuffer.bitmap_info.bmiHeader.biClrUsed = 0;
        backbuffer.bitmap_info.bmiHeader.biClrImportant = 0;
        backbuffer.bitmap_info.bmi_colors_pointer = 0;
    }
    
    // NOTE: Platform initialization
    {
        global_platform.permanent_storage_size = PERMANENT_STORAGE_SIZE;
        global_platform.permanent_storage = VirtualAlloc(0, global_platform.permanent_storage_size,
                                                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        
        if(!global_platform.permanent_storage)
        {
            DWORD error = GetLastError();
            fprintf(stderr, "ERROR: Application memory failure (%i).\n", error);
            goto quit;
        }
        
        global_platform.initialized = 0; 
        global_platform.backbuffer_width = backbuffer.width;
        global_platform.backbuffer_height = backbuffer.height;
        global_platform.backbuffer = backbuffer.memory;
        global_platform.frames_per_second_target = 60.f;
    }
    
    i64 performance_counter_frequency = 1;
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    performance_counter_frequency = freq.QuadPart;
    
    LARGE_INTEGER begin_frame_time_data;
    LARGE_INTEGER end_frame_time_data;
    
    while(!global_platform.quit)
    {
        global_platform.last_time = global_platform.current_time;
        global_platform.current_time += 1.f / global_platform.frames_per_second_target;
        i64 desired_frame_time_counts = performance_counter_frequency / global_platform.frames_per_second_target;
        QueryPerformanceCounter(&begin_frame_time_data);
        
        // NOTE: Prepare platform for update
        {
            global_platform.left_mouse_pressed = 0;
            global_platform.right_mouse_pressed = 0;
        }
        
        // Process window messages.
        MSG message;
        while(PeekMessage(&message, 0, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessage(&message);
        }
        
        global_platform.quit |= AppUpdate(&global_platform);
        
        StretchDIBits(window_device_context,
                      0, 0, global_platform.backbuffer_width, global_platform.backbuffer_height,
                      0, 0, backbuffer.width, backbuffer.height,
                      backbuffer.memory,
                      (const BITMAPINFO *)&backbuffer.bitmap_info,
                      DIB_RGB_COLORS, SRCCOPY);
        
        QueryPerformanceCounter(&end_frame_time_data);
        
        // NOTE: Wait any time, if necessary.
        // TODO: Think about changing the target FPS if the target has not
        //            been met.
        {
            i64 counts_in_frame = end_frame_time_data.QuadPart - begin_frame_time_data.QuadPart;
            i64 counts_to_wait = desired_frame_time_counts - counts_in_frame;
            
            LARGE_INTEGER begin_wait_time_data;
            LARGE_INTEGER end_wait_time_data;
            
            QueryPerformanceCounter(&begin_wait_time_data);
            
            while(counts_to_wait > 0)
            {
                QueryPerformanceCounter(&end_wait_time_data);
                counts_to_wait -= end_wait_time_data.QuadPart - begin_wait_time_data.QuadPart;
                begin_wait_time_data = end_wait_time_data;
            }
        }
    }
    
    quit:;
    
    return 0;
}