#include <Windows.h> 
#include <Xinput.h>
#include <dsound.h>
#include <stdint.h>

#include "win32_crimson.h"
#include "crimson_math.h"
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

internal debug_read_file_result DEBUGPlatformReadEntireFile(char *filename)
{
    debug_read_file_result result = {0};

    HANDLE file_handle = CreateFileA(filename, GENERIC_READ, 0, 0,
                                     OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (file_handle != INVALID_HANDLE_VALUE) 
    {
        LARGE_INTEGER file_size;
        if (GetFileSizeEx(file_handle, &file_size)) 
        {
            u32 file_size32 = SafeTruncateUInt64(file_size.QuadPart);
            result.memory = VirtualAlloc(0, file_size32, 
                                         MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
            if (result.memory) 
            {
                DWORD bytes_read;
                if (ReadFile(file_handle, result.memory, file_size32, &bytes_read, 0) && 
                    bytes_read == file_size32) 
                {
                    result.size = file_size32;
                } 
                else 
                {
                    // TODO: logging
                    DEBUGPlatformFreeFileMemory(result.memory);
                    result.memory = 0;
                }
            } 
            else 
            {
                // TODO: logging
            }
        } 
        else 
        {
            // TODO: logging
        }

        CloseHandle(file_handle);
    } 
    else
    {
        // TODO: logging
    }

    return result;
}

internal void DEBUGPlatformFreeFileMemory(void *memory)
{
    VirtualFree(memory, 0, MEM_RELEASE);
}

internal b32 DEBUGPlatformWriteFile(char *filename, u32 memory_size, void *memory)
{
    b32 result = 0;

    HANDLE file_handle = CreateFileA(filename, GENERIC_WRITE, 0, 0,
                                     CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

    if (file_handle != INVALID_HANDLE_VALUE) 
    {
        DWORD bytes_written;
        if (WriteFile(file_handle, memory, memory_size, &bytes_written, 0))
        {
            result = (bytes_written == memory_size);
        } 
        else 
        {
            // TODO: logging
        }

        CloseHandle(file_handle);
    } 
    else
    {
        // TODO: logging
    }

    return result;
}

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

internal void Win32ProcessXInputDigitalButton(game_button_state *old_state, 
                                              game_button_state *new_state, 
                                              DWORD xinput_button_state,
                                              DWORD button_bit)
{
    new_state->half_transititions = (old_state->ended_down != new_state->ended_down) ? 1 : 0;
    new_state->ended_down = (xinput_button_state & button_bit);
}

internal void Win32ProcessKeyboardButton(game_button_state *new_state, b32 is_down)
{
    if (new_state->ended_down != is_down) 
    {
        Assert(new_state->ended_down != is_down);
        new_state->ended_down = is_down;
        ++new_state->half_transititions;
    }
}

internal f32 Win32ProcessInputStickValue(SHORT value, SHORT deadzone)
{
    f32 result = 0; 

    if (value < -deadzone)
    {
        result = (f32)value / 32768.0f;
    }
    else if (value > deadzone)
    {
        result = (f32)value / 32768.0f;
    }

    return result;
}

internal void Win32ProcessInput(game_controller_input *keyboard_input)
{
    MSG message; 
    while (PeekMessageA(&message, 0, 0, 0, PM_REMOVE))
    {
        switch (message.message)
        {
            case WM_SYSKEYDOWN:
            case WM_SYSKEYUP:
            case WM_KEYDOWN:
            case WM_KEYUP:
            {
                u32 vk_code = (u32)message.wParam;
                b32 was_down = ((message.lParam & (1 << 30)) != 0);
                b32 is_down = ((message.lParam & (1 << 31)) == 0);

                u32 key_index = 0;

                if (is_down != was_down)
                {
                    /* NOTE: This is for full keyboard support
                    if(vk_code >= 'A' && vk_code <= 'Z')
                    {
                        key_index = KEY_a + (vk_code - 'A');
                        Win32ProcessKeyboardButton(&input->keyboard[key_index], is_down);
                    }
                    else if (vk_code >= '0' && vk_code <= '9')
                    {
                        key_index = KEY_0 + (vk_code - '0');
                        Win32ProcessKeyboardButton(&input->keyboard[key_index], is_down);
                    }
                    */
                    switch (vk_code)
                    {
                        case 'W': 
                        {
                        } break;

                        case 'A': 
                        {
                        } break;

                        case 'S': 
                        {
                        } break;

                        case 'D': 
                        {
                        } break;

                        case 'Q': 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->left_shoulder, is_down);
                        } break;

                        case 'E': 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->right_shoulder, is_down);
                        } break;
                        
                        case VK_UP: 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->up, is_down);
                        } break;

                        case VK_DOWN: 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->down, is_down);
                        } break;

                        case VK_LEFT: 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->left, is_down);
                        } break;
                        
                        case VK_RIGHT: 
                        {
                            Win32ProcessKeyboardButton(&keyboard_input->right, is_down);
                        } break;

                        case VK_SPACE: 
                        {
                        } break;
                        
                        case VK_ESCAPE: 
                        {
                        } break;
                    }
                }

                b32 alt_key_was_down = ((message.lParam & (1 << 29)) != 0);
                if ((vk_code == VK_F4) && alt_key_was_down)
                {
                    Running = 0; 
                }
            } break; 

            default: 
            {
                TranslateMessage(&message);
                DispatchMessageA(&message); 
            } break;
        }
    }
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
            Assert("Keyboard input detected here");
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
#if CRIMSON_INTERNAL 
            LPVOID base_address = (LPVOID)Terabytes(2);
#else 
            LPVOID base_address = 0;
#endif
            game_memory game_memory = {0};
            game_memory.storage_size = Megabytes(64); 
            game_memory.transient_storage_size = Megabytes(100);
            game_memory.storage = VirtualAlloc(0, game_memory.storage_size,
                                               MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
            game_memory.transient_storage = VirtualAlloc(0, game_memory.transient_storage_size,
                                                         MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

            game_input input[2];
            game_input *new_input = &input[0];
            game_input *old_input = &input[1];

            LARGE_INTEGER last_counter; 
            QueryPerformanceCounter(&last_counter);
            u64 last_cycle_count = __rdtsc();

            while (Running)
            {
                game_controller_input *old_keyboard_controller = &old_input->controllers[0];
                game_controller_input *new_keyboard_controller = &new_input->controllers[0];
                game_controller_input zero_controller = {0};

                *new_keyboard_controller = zero_controller;

                for (int i = 0; i < ArraySize(new_keyboard_controller->buttons); ++i)
                {
                    new_keyboard_controller->buttons[i].ended_down = 
                        old_keyboard_controller->buttons[i].ended_down;
                }

                DWORD max_controller_count = XUSER_MAX_COUNT + 1;
                if (max_controller_count > ArraySize(new_input->controllers))
                {
                    max_controller_count = ArraySize(new_input->controllers);
                }

                for (DWORD controller_index = 0; controller_index < max_controller_count; ++controller_index)
                {
                    game_controller_input *old_controller = &old_input->controllers[controller_index + 1];
                    game_controller_input *new_controller = &new_input->controllers[controller_index + 1];

                    XINPUT_STATE controller_state; 
                    if (XInputGetState(controller_index + 1, &controller_state) == ERROR_SUCCESS)
                    {
                        XINPUT_GAMEPAD *pad = &controller_state.Gamepad; 

                        new_controller->is_analog = 1;

                        b32 up = (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP);
                        b32 down = (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN);
                        b32 left = (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT);
                        b32 right = (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT);
                        b32 left_shoulder = (pad->wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER);
                        b32 Right_shoulder = (pad->wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER);
                        b32 start = (pad->wButtons & XINPUT_GAMEPAD_START);
                        b32 back = (pad->wButtons & XINPUT_GAMEPAD_BACK);
                        b32 a_button = (pad->wButtons & XINPUT_GAMEPAD_A);
                        b32 b_button = (pad->wButtons & XINPUT_GAMEPAD_B);
                        b32 x_button = (pad->wButtons & XINPUT_GAMEPAD_X);
                        b32 y_button = (pad->wButtons & XINPUT_GAMEPAD_Y);

                        /*
                        f32 x = Win32ProcessInputStickValue(pad->sThumbLX, 
                                                            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                        new_controller->min_x = new_controller->max_x = new_controller->end_x = x;

                        f32 y = Win32ProcessInputStickValue(pad->sThumbLY, 
                                                            XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                        new_controller->min_y = new_controller->max_y = new_controller->end_y = y;
                                                        */

                        new_controller->stick_average_x = Win32ProcessInputStickValue(
                            pad->sThumbLX, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);
                        new_controller->stick_average_y = Win32ProcessInputStickValue(
                            pad->sThumbLY, XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE);

/*
                        if (new_controller->stick_average_x != 0.0f ||
                            new_controller->stick_average_x != 0.0f) 
                        {
                            new_controller->is_analog = 1;
                        }

                        if (pad->wButtons & XINPUT_GAMEPAD_DPAD_UP) 
                        {
                            new_controller->stick_average_y = 1.0f;
                            new_controller->is_analog = 0;
                        }
                        if (pad->wButtons & XINPUT_GAMEPAD_DPAD_DOWN) 
                        {
                            new_controller->stick_average_y = -1.0f;
                            new_controller->is_analog = 0;
                        }
                        if (pad->wButtons & XINPUT_GAMEPAD_DPAD_RIGHT) 
                        {
                            new_controller->stick_average_y = 1.0f;
                            new_controller->is_analog = 0;
                        }
                        if (pad->wButtons & XINPUT_GAMEPAD_DPAD_LEFT) 
                        {
                            new_controller->stick_average_y = -1.0f;
                            new_controller->is_analog = 0;
                        }
                        */

                        f32 threshold = 0.5f;
                        Win32ProcessXInputDigitalButton(&old_controller->up,
                                                        &new_controller->up,
                                                        (new_controller->stick_average_y > threshold ? 1 : 0),
                                                        1);
                        Win32ProcessXInputDigitalButton(&old_controller->down,
                                                        &new_controller->down,
                                                        (new_controller->stick_average_y < -threshold ? 1 : 0),
                                                        1);
                        Win32ProcessXInputDigitalButton(&old_controller->right,
                                                        &new_controller->right,
                                                        (new_controller->stick_average_x > threshold ? 1 : 0),
                                                        1);
                        Win32ProcessXInputDigitalButton(&old_controller->left,
                                                        &new_controller->left,
                                                        (new_controller->stick_average_x < -threshold ? 1 : 0),
                                                        1);

                        Win32ProcessXInputDigitalButton(&old_controller->y_button,
                                                        &new_controller->y_button,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_Y);
                        Win32ProcessXInputDigitalButton(&old_controller->x_button,
                                                        &new_controller->x_button,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_X);
                        Win32ProcessXInputDigitalButton(&old_controller->b_button,
                                                        &new_controller->b_button,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_B);
                        Win32ProcessXInputDigitalButton(&old_controller->a_button,
                                                        &new_controller->a_button,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_A);

                        Win32ProcessXInputDigitalButton(&old_controller->left_shoulder,
                                                        &new_controller->left_shoulder,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_LEFT_SHOULDER);
                        Win32ProcessXInputDigitalButton(&old_controller->right_shoulder,
                                                        &new_controller->right_shoulder,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_RIGHT_SHOULDER);

                        Win32ProcessXInputDigitalButton(&old_controller->start,
                                                        &new_controller->start,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_START);
                        Win32ProcessXInputDigitalButton(&old_controller->back,
                                                        &new_controller->back,
                                                        pad->wButtons,
                                                        XINPUT_GAMEPAD_BACK);
                    }
                }

                Win32ProcessInput(new_keyboard_controller);

                offscreen_buffer buffer = {0};
                {
                    buffer.memory = Global_Backbuffer.memory;
                    buffer.width = Global_Backbuffer.width; 
                    buffer.height = Global_Backbuffer.height; 
                    buffer.pitch = Global_Backbuffer.pitch; 
                    buffer.bytes_per_pixel = Global_Backbuffer.bytes_per_pixel;
                }

                UpdateApp(&game_memory, &buffer, new_input);

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
                i32 ms_per_frame = (i32)((1000 * counter_elapsed) / perf_count_frequency);
                i32 fps = (i32)(perf_count_frequency / counter_elapsed);
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
