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
        void ProcessGamepadInput(RawInputData rawInput, GamepadManager& gamepadManager);

    protected:
        bool PlatformInit(PlatformCoreInitData const& initData, GamepadManager& gamepadManager);

    private:
        bool SubscribeToInputMessages();
        bool SubscribeToDeviceChanges(WindowHandle hWnd);

        // Windows gamepad
        bool GatherGamepads(GamepadManager& gamepadManager);
        bool IsDeviceGamepad(DeviceHandle device) const;
    };
    using PlatformCore = PlatformCore_Windows;
}
#endif // HIDra_Windows