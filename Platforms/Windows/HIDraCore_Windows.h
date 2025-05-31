#pragma once

#include "../../HIDraTypes.h"

#if HIDra_Windows
namespace HIDra
{
    using WindowHandle = void*;
    using DeviceHandle = void*;
    using RawInputData = void*;

    struct PlatformCoreInitData_Windows
    {
        WindowHandle m_mainWindowHandle;
    };
    using PlatformCoreInitData = PlatformCoreInitData_Windows;

    class GamepadManager;
    class KeyboardManager;

    class PlatformCore_Windows
    {
    public:
#if HIDra_Gamepad
        bool GatherGamepads(GamepadManager& gamepadManager);

        void ProcessGamepadInput(RawInputData rawInput, GamepadManager& gamepadManager);
#endif // HIDra_Gamepad

#if HIDra_Keyboard
        void ProcessKeystroke(HIDra_UInt16 virtualKeyCode, HIDra_UInt16 flags, KeyboardManager& keyboardManager);
#endif

    protected:
        bool PlatformInit(PlatformCoreInitData const& initData, GamepadManager& gamepadManager);

    private:
        bool SubscribeToInputMessages();

#if HIDra_Gamepad
        bool SubscribeToDeviceChanges(WindowHandle hWnd);

        bool IsDeviceGamepad(DeviceHandle device) const;
#endif // HIDra_Gamepad
    };
    using PlatformCore = PlatformCore_Windows;
}
#endif // HIDra_Windows