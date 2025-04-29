#pragma once

#include "../HIDraEnums.h"
#include "HIDraHIDDatabase.h"

#if HIDra_Gamepad
namespace HIDra
{
    constexpr float AxisDeadZone = 0.01f;

    struct GamepadInputData
    {
    public:
        GamepadButtonFlags GetFlags() const { return m_flags; }
        bool GetFlag(GamepadButtonFlags flag) const { return (m_flags & flag) != 0; }
        bool WasFlagSetThisFrame(GamepadButtonFlags flag) const { return (m_flagsSetThisFrame & flag) != 0; }

        Vec2f const& GetStickL() const { return m_stickL; }
        Vec2f const& GetStickR() const { return m_stickR; }

        float GetTriggerL() const { return m_triggerL; }
        float GetTriggerR() const { return m_triggerR; }

        float GetAxis(GamepadAxisID axisID) const;
        Vec2f const& GetAxis2D(GamepadAxisID axisID) const;

        void SetFlags(GamepadButtonFlags flags);

        void SetStickLX(float valueX) { m_stickL.m_x = valueX; }
        void SetStickLY(float valueY) { m_stickL.m_y = valueY; }
        void SetStickRX(float valueX) { m_stickR.m_x = valueX; }
        void SetStickRY(float valueY) { m_stickR.m_y = valueY; }

        void SetStickL(Vec2f const& value) { m_stickL = value; }
        void SetStickR(Vec2f const& value) { m_stickR = value; }

        void SetTriggerL(float value);
        void SetTriggerR(float value);

        void SetAxis(GamepadAxisID axisID, float value);
        void SetAxis(GamepadAxisID axisID, Vec2f const& value);

        inline void Flush()
        {
            m_flagsSetThisFrame = BID_NONE;
        }

    private:
        GamepadButtonFlags m_flags = BID_NONE;
        GamepadButtonFlags m_flagsSetThisFrame = BID_NONE;

        Vec2f m_stickL;
        Vec2f m_stickR;
        
        float m_triggerL = 0.0f;
        float m_triggerR = 0.0f;
    };

    class GamepadBase
    {
    public:
        GamepadBase() = default;
        GamepadBase(GamepadBase const&) = delete; // No! No copy >:(
        GamepadBase(GamepadBase&& otherGamepad) noexcept;
        
        virtual inline void Flush() { m_inputData.Flush(); }

        inline HIDra_UInt8 GetPredictedAxisCount() const;

        GamepadInputData const& GetInputData() const { return m_inputData; }

    protected:
        GamepadInputData m_inputData;

        Vendor m_vendor = 0;
        Product m_product = 0;
    };

    // Returns the expected number of axes that are sent per message for the product type
    inline HIDra_UInt8 GamepadBase::GetPredictedAxisCount() const
    {
        if (m_vendor == VID_NINTENDO)
        {
            switch (m_product)
            {
            case PID_NINTENDO_SWITCH_PRO_CONTROLLER:
                return 6;
            }
        }

        return 0;
    }
}
#endif // HIDra_Gamepad