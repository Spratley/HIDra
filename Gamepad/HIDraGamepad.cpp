#include "HIDraGamepad.h"
#if HIDra_Gamepad
#include "HIDraHIDDatabase.h"

namespace HIDra
{
    float GamepadInputData::GetAxis(GamepadAxisID axisID) const
    {
        switch (axisID)
        {
        case AID_TRIGGER_L:
            return GetTriggerL();
        case AID_TRIGGER_R:
            return GetTriggerR();
        }
        return 0.0f;
    }
    
    Vec2f const& GamepadInputData::GetAxis2D(GamepadAxisID axisID) const
    {
        switch (axisID)
        {
        case AID_STICK_L:
            return GetStickL();
        case AID_STICK_R:
            return GetStickR();
        }
        return Vec2f::s_zero;
    }

    void GamepadInputData::SetFlags(GamepadButtonFlags flags)
    {
        m_flagsSetThisFrame = static_cast<GamepadButtonFlags>(m_flags ^ flags);
        m_flags = flags;
    }

    void GamepadInputData::SetTriggerL(float value)
    {
        m_triggerL = value;
    }
    
    void GamepadInputData::SetTriggerR(float value)
    {
        m_triggerR = value;
    }

    void GamepadInputData::SetAxis(GamepadAxisID axisID, float value)
    {
        switch (axisID)
        {
        case AID_TRIGGER_L:
            SetTriggerL(value);
            break;
        case AID_TRIGGER_R:
            SetTriggerR(value);
            break;
        }
    }

    void GamepadInputData::SetAxis(GamepadAxisID axisID, Vec2f const& value)
    {
        switch (axisID)
        {
        case AID_STICK_L:
            SetStickL(value);
            break;
        case AID_STICK_R:
            SetStickR(value);
            break;
        }
    }

    GamepadBase::GamepadBase(GamepadBase&& otherGamepad) noexcept
        : m_inputData() // Clear input data- It's going to be refreshed on the next frame anyway
        , m_vendor(otherGamepad.m_vendor)
        , m_product(otherGamepad.m_product)
    {}
}
#endif // HIDra_Gamepad