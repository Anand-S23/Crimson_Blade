#include <Windows.h> 
#include <Xinput.h>
#include <dsound.h>
#include <stdint.h>

#include "win32_crimson.h"
#include "crimson_platform.h"
#include "crimson.c"

#define X_INPUT_GET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_STATE *pState)
typedef X_INPUT_GET_STATE(x_input_get_state); 
X_INPUT_GET_STATE(XInputGetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED; 
}
global x_input_get_state *XInputGetState_ = XInputGetStateStub;
#define XInputGetState XInputGetState_ 

#define X_INPUT_SET_STATE(name) DWORD WINAPI name(DWORD dwUserIndex, XINPUT_VIBRATION *pVibration)
typedef X_INPUT_SET_STATE(x_input_set_state); 
X_INPUT_SET_STATE(XInputSetStateStub)
{
    return ERROR_DEVICE_NOT_CONNECTED; 
}
global x_input_set_state *XInputSetState_ = XInputSetStateStub;
#define XInputSetState XInputSetState_ 

#define DIRECT_SOUND_CREATE(name) DWORD WINAPI name(LPCGUID pcGuidDevice, LPDIRECTSOUND *ppDS, LPUNKNOWN pUnkOuter)
typedef DIRECT_SOUND_CREATE(direct_sound_create);

global b32 Running = 1; 
global win32_offscreen_buffer Global_Backbuffer; 

/*
internal void Win32InitDSound(HWND window, int32_t samples_per_second, int32_t buffer_size) 
{
    HMODULE dsound_library = LoadLibraryA("dsound.dll");
    if (dsound_library)
    {
        direct_sound_create *DirectSoundCreate = (direct_sound_create *)GetProcAddress(dsound_library, "DirectSoundCreate");

        LPDIRECTSOUND direct_sound; 
        if (DirectSoundCreate && SUCCEEDED(DirectSoundCreate(0, &direct_sound, 0)))
        {
            WAVEFORMATEX wave_format = {0};
            wave_format.wFormatTag = WAVE_FORMAT_PCM;
            wave_format.nChannels = 2;
            wave_format.nSamplesPerSec = samples_per_second;
            wave_format.wBitsPerSample = 16;
            wave_format.nBlockAlign = wave_format.nChannels * wave_format.wBitsPerSample / 8;
            wave_format.nAvgBytesPerSec = wave_format.nSamplesPerSec * wave_format.nBlockAlign;

            if (SUCCEEDED(direct_sound->SetCooperativeLevel(window, DSSCL_PRIORITY))) 
            {
                DSBUFFERDESC buffer_desc = {0};
                buffer_desc.dwSize = sizeof(buffer_desc);
                buffer_desc.dwFlags = DSBCAPS_PRIMARYBUFFER;

                LPDIRECTSOUNDBUFFER primary_buffer;
                if (SUCCEEDED(direct_sound->CreateSoundBuffer(&buffer_description, &primary_buffer, 0))) 
                {

                    if (SUCCEEDED(primary_buffer->SetFormat(&wave_format))) 
                    {
                    }
                    else 
                    {
                        // TDOO: logging
                    }
                } 
                else 
                {
                    // TODO: logging
                }

                buffer_desc.dwBufferBytes = buffer_size;

            } 
            else 
            {
                // TODO: logging
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
}
       */

internal void Win32LoadXInput()
{
    HMODULE xinput_library = LoadLibrary("xinput1_4.dll");
    if (!xinput_library) 
    { 
        HMODULE xinput_library = LoadLibrary("xinput1_3.dll"); 
    }

    if (xinput_library)
    {
        XInputGetState = (x_input_get_state *)GetProcAddress(xinput_library, "XInputGetState"); 
        XInputSetState = (x_input_set_state *)GetProcAddress(xinput_library, "XInputSetState");
    }
    else 
    {
        // TODO: Logging 
    }
}

internal void Win32ProcessXInputDigitalButton(DWORD xinput_button_state, 
                                              game_button_state *old_state, 
                                              game_button_state *new_state, 
                                              DWORD button_bit)
{
    new_state->half_transititions = (old_state->ended_down != new_state->ended_down) ? 1 : 0;
    new_state->ended_down = (xinput_button_state & button_bit);
}

internal window_dimension GetWindowDimension(HWND window)
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
    LARGE_INTEGER perf_count_frequency_result; 
    QueryPerformanceFrequency(&perf_count_frequency_result);
    i64 perf_count_frequency = perf_count_frequency_result.QuadPart;
    
    Win32LoadXInput();
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
            game_input input[2];
            game_input *new_input = &input[0];
            game_input *old_input = &input[1];

            LARGE_INTEGER last_counter; 
            QueryPerformanceCounter(&last_counter);
            u64 last_cycle_count = __rdtsc();

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

                int max_controller_count = XUSER_MAX_COUNT;
                if (max_controller_count > ArrayCount(new_input->controllers))
                {
                    max_controller_count = ArrayCount(new_input->controllers);
                }

                for (DWORD controller_index = 0; controller_index < max_controller_count; ++controller_index)
                {
                    game_controller_input *old_controller = &old_input->controllers[controller_index];
                    game_controller_input *new_controller = &new_input->controllers[controller_index];

                    XINPUT_STATE controller_state; 
                    if (XInputGetState(controller_index, &controller_state) == ERROR_SUCCESS)
                    {
                        XINPUT_GAMEPAD *pad = &controller_state.Gamepad; 

                        b32 up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        b32 down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        b32 left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        b32 right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        b32 left_shoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        b32 Right_shoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        int16_t stick_x = pad->sThumbLX;
                        int16_t stick_y = pad->sThumbLY;

                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->up, &new_controller->up, 
                                                        XINPUT_GAMEPAD_Y);
                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->down, &new_controller->down, 
                                                        XINPUT_GAMEPAD_A);
                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->right, &new_controller->right, 
                                                        XINPUT_GAMEPAD_B);
                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->left, &new_controller->left, 
                                                        XINPUT_GAMEPAD_X);
                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->left_shoulder, &new_controller->left_shoulder, 
                                                        XINPUT_GAMEPAD_LEFT_SHOULDER);
                        Win32ProcessXInputDigitalButton(pad->wButtons, 
                                                        &old_controller->right_shoulder, &new_controller->right_shoulder, 
                                                        XINPUT_GAMEPAD_RIGHT_SHOULDER);

                        b32 start = (pad->wButtons & XINPUT_GAMEPAD_START);
                        b32 back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
                        b32 a_button = (pad->wButtons & XINPUT_GAMEPAD_A);
                        b32 b_button = (pad->wButtons & XINPUT_GAMEPAD_B);
                        b32 x_button = (pad->wButtons & XINPUT_GAMEPAD_X);
                        b32 y_button = (pad->wButtons & XINPUT_GAMEPAD_Y);

                    }
                }

                offscreen_buffer buffer = {0};
                {
                    buffer.memory = Global_Backbuffer.memory;
                    buffer.width = Global_Backbuffer.width; 
                    buffer.height = Global_Backbuffer.height; 
                    buffer.pitch = Global_Backbuffer.pitch; 
                    buffer.bytes_per_pixel = Global_Backbuffer.bytes_per_pixel;
                }

                UpdateApp(&buffer, new_input);

                HDC device_context = GetDC(window);
                window_dimension dimension = GetWindowDimension(window);
                Win32DisplayBufferInWindow(device_context,
                                           dimension.width, dimension.height,
                                           &Global_Backbuffer);
                ReleaseDC(window, device_context);

                u64 end_cycle_count = __rdtsc();

                LARGE_INTEGER end_counter;
                QueryPerformanceCounter(&end_counter);

                u64 cycle_elpased = end_cycle_count - last_cycle_count;
                i64 counter_elapsed = end_counter.QuadPart - last_counter.QuadPart; 
                i32 ms_per_frame = (1000 * counter_elapsed) / perf_count_frequency;
                i32 fps = perf_count_frequency / counter_elapsed;
                i32 mcpf = (i32)(cycle_elpased / (1000 * 1000));

                char str_buffer[256];
                wsprintf(str_buffer, "%dms/f - %dFPS - %dmc/f\n", ms_per_frame, fps, mcpf);
                //OutputDebugStringA(str_buffer);

                last_counter = end_counter;
                last_cycle_count = end_cycle_count;

                game_input *temp = new_input; 
                new_input = old_input; 
                old_input = temp;
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
