#include "HIDraCore.h"

#if HIDra_Platform == HIDra_Platform_Windows
#include "../Gamepad/HIDraGamepad.h"
#include "../HIDraTypes.h"

#include <Windows.h>
// Windows.h MUST be included first
#include <dbt.h>
#include <hidsdi.h>
#include <hidusage.h>

#pragma comment(lib, "hid.lib")

// TODO: Don't do this
#if HIDra_Debug
#define PRINT_WINDOWS_ERROR(ErrorMessage) printf(ErrorMessage " Windows Error Code: %i\n", GetLastError())
#define PRINT_ERROR(ErrorMessageFmt, ...) printf(ErrorMessageFmt "\n", __VA_ARGS__)
#else
#define PRINT_WINDOWS_ERROR(ErrorMessage)
#define PRINT_ERROR(ErrorMessageFmt, ...)
#endif // HIDra_Debug

#include "../PlatformUtils/Windows/HIDraWindowsGamepadUtils.inl"

namespace
{
#include "../PlatformUtils/Windows/HIDraWindowsKeyboardUtils.inl"

    // TODO: Clean this up so it's less nested and easier to read at a glance
    // Hijack the windows message handler to filter input messages through HIDra
    WNDPROC _defaultWindowProcedure;
    LRESULT CALLBACK WindowsRawInputMessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
    {
        // TODO: Come back and see if this needs a refactoring
        if (message == WM_INPUT)
        {
            // Ideally this data would be passed off to an input handler so we're not blocking the windows message
            // In my own testing a blank opengl app runs at 700 fps on my potato laptop without any magic handoff and
            // async processing. Still it's a stretch goal, but not right now

            // Check data size
            UINT inputMessageSize;
            GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &inputMessageSize, sizeof(RAWINPUTHEADER));
            LPBYTE messageBuffer = new BYTE[inputMessageSize];

            // Read data
            UINT expectedSize = inputMessageSize;
            if (expectedSize
                == GetRawInputData((HRAWINPUT)lParam,
                                   RID_INPUT,
                                   messageBuffer,
                                   &inputMessageSize,
                                   sizeof(RAWINPUTHEADER)))
            {
                RAWINPUT* rawData = (RAWINPUT*)messageBuffer;

#if HIDra_Keyboard
                if (rawData->header.dwType == RIM_TYPEKEYBOARD)
                {
                    // TODO: Determine if the pressed shift key is left or right.
                    // RAWINPUT doesn't differentiate left and right shift with flags like it does for left and right
                    // alt We need to check directly via scancodes, but this setup isn't exactly built around that
                    RAWKEYBOARD const& keyboardData = rawData->data.keyboard;
                    ProcessVirtualKeystroke(keyboardData.VKey, keyboardData.Flags);
                }
#endif // HIDra_Keyboard

#if HIDra_Gamepad
                if (rawData->header.dwType == RIM_TYPEHID)
                {
                    HIDra::GamepadHandler& gamepadHandler = HIDra::Core::GetInstance().GetGamepadHandler();
                    if (gamepadHandler.HasGamepadsConnected())
                    {
                        HIDra::GamepadInputData inputReport;
                        HIDra::GamepadID const gamepadID = ProcessGamepadRawInputData(rawData, inputReport);
                        gamepadHandler.ReportInput(gamepadID, inputReport);
                    }
                }
#endif // HIDra_Gamepad
            }
            else
            {
                PRINT_WINDOWS_ERROR("GetRawInputData failed to return correct size!");
            }
            delete[] messageBuffer;
        }
        else if (message == WM_DEVICECHANGE)
        {
#if HIDra_Gamepad && HIDra_GP_Multiple
            // TODO: Process device change messages better
            HIDra::GamepadHandler& gamepadHandler = HIDra::Core::GetInstance().GetGamepadHandler();
            gamepadHandler.GatherAllGamepads();
#endif // HIDra_Gamepad && HIDra_GP_Multiple
        }

        // Pass forward event to previous window procedure for remaining handling
        return CallWindowProc(_defaultWindowProcedure, windowHandle, message, wParam, lParam);
    }

    bool SubscribeToInputMessages()
    {
        // Equal to the number of enabled input methods
        constexpr HIDra::HIDra_UInt32 rawInputDeviceTypeCount = HIDra_Keyboard + HIDra_Mouse + HIDra_Gamepad;

        RAWINPUTDEVICE rid[rawInputDeviceTypeCount];
        HIDra::HIDra_UInt32 index = 0;

#if HIDra_Keyboard
        rid[index].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[index].usUsage = HID_USAGE_GENERIC_KEYBOARD;
        rid[index].dwFlags = 0;
        rid[index].hwndTarget = 0;
        index++;
#endif // HIDra_Keyboard

#if HIDra_Mouse
        rid[index].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[index].usUsage = HID_USAGE_GENERIC_MOUSE;
        rid[index].dwFlags = 0;
        rid[index].hwndTarget = 0;
        index++;
#endif // HIDra_Mouse

#if HIDra_Gamepad
        rid[index].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[index].usUsage = HID_USAGE_GENERIC_GAMEPAD;
        rid[index].dwFlags = 0;
        rid[index].hwndTarget = 0;
        index++;
#endif // HIDra_Gamepad

        if (RegisterRawInputDevices(rid, rawInputDeviceTypeCount, sizeof(rid[0])) == FALSE)
        {
            PRINT_WINDOWS_ERROR("Failed to register RawInput devices!");
            return false;
        }

        return true;
    }

    bool InitGamepadHandler(HIDra::GamepadHandler& gamepadHandler, HWND hWnd)
    {
#if HIDra_Gamepad
        if (!SubscribeToDeviceChanges(hWnd))
        {
            return false;
        }

#if HIDra_GP_Multiple
        return gamepadHandler.GatherAllGamepads();
#else
        static_cast<void*>(gamepadHandler);
        return true;
#endif // HIDra_GP_Multiple
#endif // HIDra_Gamepad
    }

} // namespace

namespace HIDra
{
    bool Core::Init(Core_PlatformInitData const& initData)
    {
        HWND hWnd = (HWND)initData.m_mainWindowHandle;
        // TODO: DON'T DO THIS! It's unsafe to have an unmanaged stack like this
        // If HIDra is unloaded, then another part of the program overloads the windows procedure
        // there's a chance they can override each-other! Figure out a better way to do this
        _defaultWindowProcedure =
          (WNDPROC)(SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowsRawInputMessageHandler));

        if (!SubscribeToInputMessages())
        {
#if HIDra_Debug
            printf("Failed to subscribe to input messages!");
#endif // HIDra_Debug
            return false;
        }

        return InitGamepadHandler(m_gamepadHandler, hWnd);
    }

    void Core::PostFlush()
    {
        // Windows needs nothing extra after flushing
    }
} // namespace HIDra

#undef PRINT_ERROR
#undef PRINT_WINDOWS_ERROR
#endif // HIDra_Platform == HIDra_Platform_Windows