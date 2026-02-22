#pragma once

#include "../../../HIDraEnums.h"

#if HIDra_Platform == HIDra_Windows
namespace HIDra
{
    KeyCode GetKeycodeFromWindowsVK(HIDra_UInt16 windowsKeyCode, HIDra_UInt16 flags);
}
#endif // HIDra_Platform == HIDra_Windows