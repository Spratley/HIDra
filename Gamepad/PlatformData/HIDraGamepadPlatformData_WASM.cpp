#include "HIDraGamepadPlatformData.h"
#if HIDra_Platform == HIDra_Platform_WebAssembly

namespace HIDra
{
    GamepadPlatformData::GamepadPlatformData(GamepadPlatformData&& platformSpecificData) noexcept {}

    GamepadPlatformData::~GamepadPlatformData() {}

    GamepadPlatformData& GamepadPlatformData::operator=(GamepadPlatformData&& platformSpecificData) noexcept
    {
        return *this;
    }

} // namespace HIDra

#endif // HIDra_Platform == HIDra_Platform_WebAssembly