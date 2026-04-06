#include "HIDraGamepadPlatformData.h"
#if HIDra_Platform == HIDra_Platform_Windows

#include <Windows.h>
#include <hidsdi.h>

#pragma comment(lib, "hid.lib")

namespace HIDra
{
    GamepadPlatformData::GamepadPlatformData(GamepadPlatformData&& platformSpecificData) noexcept
        : m_openDeviceHandle(platformSpecificData.m_openDeviceHandle)
        , m_lastKnownDeviceHandle(platformSpecificData.m_lastKnownDeviceHandle)
        , m_deviceSerialNumber(platformSpecificData.m_deviceSerialNumber)
        , m_reportSize(platformSpecificData.m_reportSize)
        , m_valueCapabilitiesCount(platformSpecificData.m_valueCapabilitiesCount)
        , m_buttonCapabilities(platformSpecificData.m_buttonCapabilities)
        , m_valueCapabilities(platformSpecificData.m_valueCapabilities)
        , m_preparsedData(platformSpecificData.m_preparsedData)
    {
        platformSpecificData.m_openDeviceHandle = INVALID_HANDLE_VALUE;
        platformSpecificData.m_lastKnownDeviceHandle = INVALID_HANDLE_VALUE;
        platformSpecificData.m_deviceSerialNumber = L"";
        platformSpecificData.m_reportSize = 0;
        platformSpecificData.m_valueCapabilitiesCount = 0;
        platformSpecificData.m_buttonCapabilities = nullptr;
        platformSpecificData.m_valueCapabilities = nullptr;
        platformSpecificData.m_preparsedData = nullptr;
    }

    GamepadPlatformData::~GamepadPlatformData()
    {
        if (m_openDeviceHandle != INVALID_HANDLE_VALUE)
        {
            CloseHandle(m_openDeviceHandle);
        }

        delete[] m_buttonCapabilities;
        m_buttonCapabilities = nullptr;
        delete[] m_valueCapabilities;
        m_valueCapabilities = nullptr;

        if (m_preparsedData)
        {
            HidD_FreePreparsedData(static_cast<PHIDP_PREPARSED_DATA>(m_preparsedData));
            m_preparsedData = nullptr;
        }
    }

    GamepadPlatformData& GamepadPlatformData::operator=(GamepadPlatformData&& platformSpecificData) noexcept
    {
        m_openDeviceHandle = platformSpecificData.m_openDeviceHandle;
        m_lastKnownDeviceHandle = platformSpecificData.m_lastKnownDeviceHandle;
        m_deviceSerialNumber = platformSpecificData.m_deviceSerialNumber;
        m_reportSize = platformSpecificData.m_reportSize;
        m_valueCapabilitiesCount = platformSpecificData.m_valueCapabilitiesCount;
        m_buttonCapabilities = platformSpecificData.m_buttonCapabilities;
        m_valueCapabilities = platformSpecificData.m_valueCapabilities;
        m_preparsedData = platformSpecificData.m_preparsedData;

        platformSpecificData.m_openDeviceHandle = INVALID_HANDLE_VALUE;
        platformSpecificData.m_lastKnownDeviceHandle = INVALID_HANDLE_VALUE;
        platformSpecificData.m_deviceSerialNumber = L"";
        platformSpecificData.m_reportSize = 0;
        platformSpecificData.m_valueCapabilitiesCount = 0;
        platformSpecificData.m_buttonCapabilities = nullptr;
        platformSpecificData.m_valueCapabilities = nullptr;
        platformSpecificData.m_preparsedData = nullptr;

        return *this;
    }

} // namespace HIDra

#endif // HIDra_Platform == HIDra_Platform_Windows