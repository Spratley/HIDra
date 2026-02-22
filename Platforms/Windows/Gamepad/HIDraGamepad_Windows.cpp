#include "HIDraGamepad_Windows.h"
#if (HIDra_Platform == HIDra_Windows) && HIDra_Gamepad
#include "HIDraGamepadDecoder_Windows.h"

#include <windows.h>
#include <hidsdi.h>

#pragma comment(lib, "hid.lib")

#if HIDra_Debug
#define PRINT_WINDOWS_ERROR(ErrorMessage) printf(ErrorMessage " Windows Error Code: %i\n", GetLastError())
#else
#define PRINT_WINDOWS_ERROR(ErrorMessage)
#endif // HIDra_Debug

namespace HIDra
{
    Gamepad_Windows::Gamepad_Windows()
        : m_reportSize(0)
        , m_openDeviceHandle(nullptr)
        , m_lastKnownDeviceHandle(nullptr)
        , m_deviceSerialNumber(L"")
        , m_valueCapabilitiesCount(0)
        , m_buttonCapabilities(nullptr)
        , m_valueCapabilities(nullptr)
        , m_preparsedData(nullptr)
    {}

    Gamepad_Windows::Gamepad_Windows(DeviceHandle deviceHandle)
        : m_lastKnownDeviceHandle(nullptr)
    {
        if (!InitWindowsGamepad(deviceHandle))
        {
            CloseHandle(m_openDeviceHandle);
            m_openDeviceHandle = INVALID_HANDLE_VALUE;
        }
    }

    Gamepad_Windows::Gamepad_Windows(Gamepad_Windows&& otherGamepad) noexcept
        : GamepadBase(std::move(otherGamepad))
        , m_reportSize(otherGamepad.m_reportSize)
        , m_openDeviceHandle(otherGamepad.m_openDeviceHandle)
        , m_lastKnownDeviceHandle(otherGamepad.m_lastKnownDeviceHandle)
        , m_deviceSerialNumber(std::move(otherGamepad.m_deviceSerialNumber))
        , m_valueCapabilitiesCount(otherGamepad.m_valueCapabilitiesCount)
        , m_buttonCapabilities(otherGamepad.m_buttonCapabilities)
        , m_valueCapabilities(otherGamepad.m_valueCapabilities)
        , m_preparsedData(otherGamepad.m_preparsedData)
    {
        otherGamepad.m_reportSize = 0;
        otherGamepad.m_openDeviceHandle = INVALID_HANDLE_VALUE;
        otherGamepad.m_lastKnownDeviceHandle = INVALID_HANDLE_VALUE;
        otherGamepad.m_deviceSerialNumber = L"";
        otherGamepad.m_valueCapabilitiesCount = 0;
        otherGamepad.m_buttonCapabilities = nullptr;
        otherGamepad.m_valueCapabilities = nullptr;
        otherGamepad.m_preparsedData = nullptr;
    }

    Gamepad_Windows::~Gamepad_Windows()
    {
        if (IsValid())
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

    bool Gamepad_Windows::InitWindowsGamepad(DeviceHandle deviceHandle)
    {
        m_openDeviceHandle = deviceHandle;

        wchar_t serialNumberBuffer[256] = {};
        if (!HidD_GetSerialNumberString(m_openDeviceHandle, serialNumberBuffer, sizeof(serialNumberBuffer)))
        {
            PRINT_WINDOWS_ERROR("HidD_GetSerialNumberString failed!");
            m_deviceSerialNumber = L"";
        }
        else
        {
            m_deviceSerialNumber = serialNumberBuffer;
        }

        HIDD_ATTRIBUTES attributes;
        if (!HidD_GetAttributes(m_openDeviceHandle, &attributes))
        {
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            return false;
        }

        PHIDP_PREPARSED_DATA preparsedData;
        if (!HidD_GetPreparsedData(m_openDeviceHandle, &preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidD_GetPreparsedData failed!");
            return false;
        }

        HIDP_CAPS capabilities;
        if (!HidP_GetCaps(preparsedData, &capabilities))
        {
            PRINT_WINDOWS_ERROR("HidP_GetCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        m_reportSize = capabilities.InputReportByteLength;
        m_vendor = attributes.VendorID;
        m_product = attributes.ProductID;

        // Button Capabilities
        HIDra_UInt16 buttonCapabilitiesCount = capabilities.NumberInputButtonCaps;
        if (buttonCapabilitiesCount == 0)
        {
            PRINT_WINDOWS_ERROR("Gamepad has 0 buttons!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        m_buttonCapabilities = new HIDP_BUTTON_CAPS[buttonCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS != HidP_GetButtonCaps(HidP_Input, static_cast<PHIDP_BUTTON_CAPS>(m_buttonCapabilities), &buttonCapabilitiesCount, preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetButtonCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        // Value Capabilities
        m_valueCapabilitiesCount = capabilities.NumberInputValueCaps;
        if (m_valueCapabilitiesCount == 0)
        {
            PRINT_WINDOWS_ERROR("Gamepad has 0 axes!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        m_valueCapabilities = new HIDP_VALUE_CAPS[m_valueCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS != HidP_GetValueCaps(HidP_Input, static_cast<PHIDP_VALUE_CAPS>(m_valueCapabilities), &m_valueCapabilitiesCount, preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetValueCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        m_preparsedData = preparsedData;
        return true;
    }

    void Gamepad_Windows::ReportInput(GamepadReport_Windows const& report)
    {
        if (m_vendor == VID_NINTENDO)
        {
            switch (m_product)
            {
            case PID_NINTENDO_SWITCH_PRO_CONTROLLER:
                GamepadDecoderNSP::Decode(report, m_inputData);
            }
        }

        // TODO: Figure out a good way to handle unknown devices so we can still read something?
    }

    bool Gamepad_Windows::IsValid() const
    {
        return m_openDeviceHandle != INVALID_HANDLE_VALUE;
    }
} // namespace HIDra
#endif // (HIDra_Platform == HIDra_Windows) && HIDra_Gamepad