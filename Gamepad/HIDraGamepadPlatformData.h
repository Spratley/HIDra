#pragma once
#include "../HIDraTypes.h"

#if HIDra_Platform == HIDra_Windows
#include <string>
#endif // HIDra_Platform == HIDra_Windows

// I'm not the biggest fan of this pattern
// Maybe come back to this?
namespace HIDra
{
    struct GamepadPlatformData
    {
        GamepadPlatformData() = default;
        GamepadPlatformData(GamepadPlatformData const&) = delete;
        GamepadPlatformData(GamepadPlatformData&& platformSpecificData) noexcept;
        ~GamepadPlatformData();

        GamepadPlatformData& operator=(GamepadPlatformData&) = delete;
        GamepadPlatformData& operator=(GamepadPlatformData&& platformSpecificData) noexcept;

#if HIDra_Platform == HIDra_Windows
        using DeviceHandle = void*;
        using ButtonCapabilitiesPtr = void*;
        using ValueCapabilitiesPtr = void*;
        using PreparsedDataPtr = void*;

        DeviceHandle m_openDeviceHandle = nullptr;
        DeviceHandle m_lastKnownDeviceHandle = nullptr;

        std::wstring m_deviceSerialNumber = L"";

        HIDra_UInt32 m_reportSize = 0;

        // Capabilities
        HIDra_UInt16 m_valueCapabilitiesCount = 0;

        ButtonCapabilitiesPtr m_buttonCapabilities = nullptr;
        ValueCapabilitiesPtr m_valueCapabilities = nullptr;
        PreparsedDataPtr m_preparsedData = nullptr;
#elif HIDra_Platform == HIDra_WebAssembly
        // No data needed yet...
#endif
    };
} // namespace HIDra