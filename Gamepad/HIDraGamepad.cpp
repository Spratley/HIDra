#include "HIDraGamepad.h"
#if HIDra_Gamepad
#include "HIDraHIDDatabase.h"

namespace HIDra
{
    void GamepadInputData::SetFlags(GamepadButtonFlags flags)
    {
        m_flagsSetThisFrame = static_cast<GamepadButtonFlags>(m_flags ^ flags);
        m_flags = flags;
    }

    void GamepadInputData::SetTriggerL(float value)
    {
        const bool isTriggered = value > 0.5f;
        const bool wasTriggered = m_triggerL > 0.5f;
        m_triggerLChangedThisFrame = isTriggered != wasTriggered;
        m_triggerL = value;
    }
    
    void GamepadInputData::SetTriggerR(float value)
    {
        const bool isTriggered = value > 0.5f;
        const bool wasTriggered = m_triggerR > 0.5f;
        m_triggerRChangedThisFrame = isTriggered != wasTriggered;
        m_triggerR = value;
    }

    GamepadBase::GamepadBase(GamepadBase&& otherGamepad) noexcept
        : m_inputData() // Clear input data- It's going to be refreshed on the next frame anyway
        , m_vendor(otherGamepad.m_vendor)
        , m_product(otherGamepad.m_product)
    {}
}
#endif // HIDra_Gamepad