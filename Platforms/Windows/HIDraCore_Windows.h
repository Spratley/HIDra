#pragma once

#include "../../HIDraDefs.h"

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
    class PlatformCore_Windows
    {
    public:
#if HIDra_Gamepad
        void ProcessGamepadInput(RawInputData rawInput, GamepadManager& gamepadManager);
#endif // HIDra_Gamepad

    protected:
        bool PlatformInit(PlatformCoreInitData const& initData, GamepadManager& gamepadManager);

    private:
        bool SubscribeToInputMessages();

#if HIDra_Gamepad
        bool SubscribeToDeviceChanges(WindowHandle hWnd);

        bool GatherGamepads(GamepadManager& gamepadManager);
        bool IsDeviceGamepad(DeviceHandle device) const;
#endif // HIDra_Gamepad
    };
    using PlatformCore = PlatformCore_Windows;
}
#endif // HIDra_Windows