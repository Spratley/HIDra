#pragma once
#include "../../../HIDraTypes.h"

#if (HIDra_Platform == HIDra_Windows) && HIDra_Gamepad
#include <vector>

namespace HIDra
{
    struct GamepadInputData;

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
                : m_id(id)
                , m_value(value)
            {}

            HIDra_UInt16 m_id;
            HIDra_UInt16 m_value;
        };

        std::vector<HIDra_UInt16> m_heldButtonIDs;
        std::vector<Axis> m_axes;
        Vendor m_vendorID;
        Product m_productID;
    };

    void DecodeWindowsReport(GamepadReport_Windows const& windowsReport, GamepadInputData& outInputData);
} // namespace HIDra
#endif // (HIDra_Platform == HIDra_Windows) && HIDra_Gamepad