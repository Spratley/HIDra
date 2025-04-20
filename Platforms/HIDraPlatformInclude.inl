#include "../HIDraDefs.h"
#if HIDra_Windows
#include "Windows/HIDraCore_Windows.h"
#include "Windows/Gamepad/HIDraGamepad_Windows.h"
#elif false
// TODO: Add other platforms
#else
#include "../Gamepad/HIDraGamepad.h"
namespace HIDra
{
    // Empty PlatformCore to let code compile
    struct PlatformCoreInitData {};
    class PlatformCore 
    {
    protected:
        bool PlatformInit() { return true; }
    };

    using Gamepad = GamepadBase;
}
#endif