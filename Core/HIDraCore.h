#pragma once

#include "../Defs/HIDraDefs.h"
#include "../Gamepad/Handler/HIDraGamepadHandler.h"
#include "../Keyboard/HIDraKeyboardHandler.h"

namespace HIDra
{
    struct Core_PlatformInitData
    {
#if HIDra_Platform == HIDra_Platform_Windows
        using WindowHandle = void*;
        WindowHandle m_mainWindowHandle;
#elif HIDra_Platform == HIDra_Platform_WebAssembly
        // No data needed... Yet...
#endif
    };

    class Core
    {
    public:
        static Core& GetInstance() { return *s_instance; }
        static Core const& GetInstanceConst() { return *s_instance; }

    private:
        static Core* s_instance;

    public:
        Core();
        bool Init(Core_PlatformInitData const& initData);
        void Flush();

#if HIDra_Keyboard
    public:
        KeyboardHandler& GetKeyboardHandler() { return m_keyboardHandler; }
        KeyboardHandler const& GetKeyboardHandler() const { return m_keyboardHandler; }

    private:
        KeyboardHandler m_keyboardHandler;
#endif // HIDra_Keyboard

#if HIDra_Gamepad
    public:
        GamepadHandler& GetGamepadHandler() { return m_gamepadHandler; }
        GamepadHandler const& GetGamepadHandler() const { return m_gamepadHandler; }

    private:
        GamepadHandler m_gamepadHandler;
#endif // HIDra_Gamepad
    };
} // namespace HIDra