#include "HIDraCore.h"

// Reserved for generic functions that stay the same across all platforms

namespace HIDra
{
    Core* Core::s_instance = nullptr;

    Core::Core()
    {
#if HIDra_Debug
        if (s_instance)
        {
            printf("Creating an instance of HIDra::Core when one already exists!");
            return;
        }
#endif // HIDra_Debug
        s_instance = this;
    }

    void Core::Flush()
    {
#if HIDra_Keyboard
        m_keyboardHandler.Flush();
#endif // HIDra_Keyboard
#if HIDra_Gamepad
        m_gamepadHandler.Flush();
#endif // HIDra_Gamepad
    }
} // namespace HIDra