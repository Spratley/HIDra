#pragma once
#include "../../../HIDraDefs.h"

#include "../../../Gamepad/HIDraGamepad.h"

#include <string>
#include <vector>

#if HIDra_Windows
namespace HIDra
{
    using DeviceHandle = void*;
    using ButtonCapabilitiesPtr = void*;
    using ValueCapabilitiesPtr = void*;
    using PreparsedDataPtr = void*;

    struct GamepadReport_Windows
    {
        struct Axis
        {
            Axis() = delete;
            Axis(const unsigned short& id, const unsigned short& value)
                : m_id(id), m_value(value) {}

            HIDra_UInt16 m_id;
            HIDra_UInt16 m_value;
        };

        // Is there something better than vectors for this?
        std::vector<HIDra_UInt16> m_heldButtonIDs;
        std::vector<Axis> m_axes;
    };

    class Gamepad_Windows : public GamepadBase
    {
    public:
        Gamepad_Windows();
        Gamepad_Windows(DeviceHandle deviceHandle);
        Gamepad_Windows(Gamepad_Windows const&) = delete; // I SAID NO COPY >:(
        Gamepad_Windows(Gamepad_Windows&& otherGamepad) noexcept;
        ~Gamepad_Windows();

        bool InitWindowsGamepad(DeviceHandle deviceHandle);

        void ReportInput(GamepadReport_Windows const& report);
    
        bool IsValid() const;

        DeviceHandle GetOpenDeviceHandle() const { return m_openDeviceHandle; }
        DeviceHandle GetLastKnownDeviceHandle() const { return m_lastKnownDeviceHandle; }
        void SetLastKnownDeviceHandle(DeviceHandle deviceHandle) { m_lastKnownDeviceHandle = deviceHandle; }
    
        std::wstring const& GetSerialNumber() const { return m_deviceSerialNumber; }

        ButtonCapabilitiesPtr const& GetButtonCapabilities() const { return m_buttonCapabilities; }
        HIDra_UInt16 GetValueCapabilitiesCount() const { return m_valueCapabilitiesCount; }
        ValueCapabilitiesPtr const& GetValueCapabilities() const { return m_valueCapabilities; }

        PreparsedDataPtr const& GetPreparsedData() const { return m_preparsedData; }

    private:
        HIDra_UInt32 m_reportSize;

        DeviceHandle m_openDeviceHandle;
        DeviceHandle m_lastKnownDeviceHandle;

        std::wstring m_deviceSerialNumber;

        // Capabilities
        HIDra_UInt16 m_valueCapabilitiesCount;

        ButtonCapabilitiesPtr m_buttonCapabilities;
        ValueCapabilitiesPtr m_valueCapabilities;
        PreparsedDataPtr m_preparsedData;
    };

    using Gamepad = Gamepad_Windows;
}
#endif // HIDra_Windows