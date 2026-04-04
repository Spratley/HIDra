#pragma once

#include "../HIDraEnums.h"
#include "HIDraGamepadPlatformData.h"

#if HIDra_Gamepad
namespace HIDra
{
    constexpr float AxisDeadZone = 0.01f;

    // Is there a way to generalize this?
    // Not all gamepads have the same input data
    // What about non-standard gamepads? Fightsticks?
    struct GamepadInputData
    {
        Vec2f m_stickL;
        Vec2f m_stickR;
        float m_triggerL = 0.0f;
        float m_triggerR = 0.0f;
        GamepadButtonFlags m_buttonFlags = BID_NONE;
    };

    class Gamepad
    {
    public:
        Gamepad() = default;
        Gamepad(Gamepad&) = delete;
        Gamepad(Gamepad&& otherGamepad)
            : m_vendor(otherGamepad.m_vendor)
            , m_product(otherGamepad.m_product)
            , m_id(otherGamepad.m_id)
            , m_platformSpecificData(std::move(otherGamepad.m_platformSpecificData))
        {
            otherGamepad.m_id = GamepadID::InvalidGamepadID;
        }

        Gamepad(Vendor vendor, Product product, GamepadID id, GamepadPlatformData&& platformSpecificData)
            : m_vendor(vendor)
            , m_product(product)
            , m_id(id)
            , m_platformSpecificData(std::move(platformSpecificData))
        {}

        void Report(GamepadInputData const& report);

        inline void Flush()
        {
            m_buttonFlagsSetThisFrame = BID_NONE;
            m_inputReportsThisFrame = 0;
        }

        Vendor GetVendorID() const { return m_vendor; }
        Product GetProductID() const { return m_product; }
        GamepadID GetID() const { return m_id; }

        inline float GetAxis(GamepadAxisID axis) const;
        inline Vec2f GetAxis2D(GamepadAxisID axis) const;

        inline bool GetButton(GamepadButtonFlags button) const;
        inline bool GetButtonDown(GamepadButtonFlags button) const;
        inline bool GetButtonUp(GamepadButtonFlags button) const;

        GamepadPlatformData const& GetPlatformSpecificData() const { return m_platformSpecificData; }

    private:
        GamepadInputData m_inputData;
        GamepadButtonFlags m_buttonFlagsSetThisFrame = BID_NONE;
        HIDra_UInt16 m_inputReportsThisFrame = 0;

        Vendor m_vendor = 0x0000;
        Product m_product = 0x0000;
        // TODO: Come back to this when adding ID persistence
        GamepadID m_id = GamepadID::InvalidGamepadID;

        GamepadPlatformData m_platformSpecificData;
    };

    float Gamepad::GetAxis(GamepadAxisID axis) const
    {
        switch (axis)
        {
            case GamepadAxisID::AID_TRIGGER_L: return m_inputData.m_triggerL;
            case GamepadAxisID::AID_TRIGGER_R: return m_inputData.m_triggerR;
            default:                           return 0.0f;
        }
    }

    Vec2f Gamepad::GetAxis2D(GamepadAxisID axis) const
    {
        switch (axis)
        {
            case GamepadAxisID::AID_STICK_L: return m_inputData.m_stickL;
            case GamepadAxisID::AID_STICK_R: return m_inputData.m_stickR;
            default:                         return Vec2f::s_zero;
        }
    }

    bool Gamepad::GetButton(GamepadButtonFlags button) const { return m_inputData.m_buttonFlags & button; }
    bool Gamepad::GetButtonDown(GamepadButtonFlags button) const
    {
        return GetButton(button) && (m_buttonFlagsSetThisFrame & button);
    }
    bool Gamepad::GetButtonUp(GamepadButtonFlags button) const
    {
        return !GetButton(button) && (m_buttonFlagsSetThisFrame & button);
    }
} // namespace HIDra
#endif // HIDra_Gamepad