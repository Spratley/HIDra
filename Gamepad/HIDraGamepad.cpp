#include "HIDraGamepad.h"
#if HIDra_Gamepad

namespace HIDraGamepad_Private
{
    template <typename ValueType>
    inline ValueType AddAveragedValue(ValueType const& previousAverage,
                                      HIDra::HIDra_UInt32 previousCount,
                                      ValueType const& value)
    {
        return ((previousAverage * previousCount) + value) / (previousCount + 1);
    }

    // TODO: Expose as a parameter to be configured?
    constexpr float DeadZone = 0.05f;

    constexpr float SanitizeFloat(float value) 
    {
        // MSVC doesn't have a constexpr std::abs()...
        if ((value < 0.0f && value > -DeadZone) || (value > 0.0f && value < DeadZone))
        {
            return 0.0f;
        }
        return value;
    }

    constexpr HIDra::Vec2f SanitizeStick(HIDra::Vec2f const& stickValue)
    {
        return HIDra::Vec2f{ SanitizeFloat(stickValue.m_x), SanitizeFloat(stickValue.m_y) };
    }
} // namespace HIDraGamepad_Private

namespace HIDra
{
    void Gamepad::Report(GamepadInputData const& report)
    {
        if (m_inputReportsThisFrame == 0)
        {
            m_buttonFlagsSetThisFrame =
              static_cast<GamepadButtonFlags>(m_inputData.m_buttonFlags ^ report.m_buttonFlags);
            m_inputReportsThisFrame++;
            m_inputData = report;
            m_inputData.m_stickL = HIDraGamepad_Private::SanitizeStick(report.m_stickL);
            m_inputData.m_stickR = HIDraGamepad_Private::SanitizeStick(report.m_stickR);
            return;
        }

        // Raise additional flags for any buttons changed by this report
        m_buttonFlagsSetThisFrame = static_cast<GamepadButtonFlags>(
          m_buttonFlagsSetThisFrame | (m_inputData.m_buttonFlags ^ report.m_buttonFlags));
        m_inputData.m_buttonFlags = report.m_buttonFlags;

        HIDraGamepad_Private::AddAveragedValue(m_inputData.m_stickL,
                                               m_inputReportsThisFrame,
                                               HIDraGamepad_Private::SanitizeStick(report.m_stickL));
        HIDraGamepad_Private::AddAveragedValue(m_inputData.m_stickR,
                                               m_inputReportsThisFrame,
                                               HIDraGamepad_Private::SanitizeStick(report.m_stickR));
        HIDraGamepad_Private::AddAveragedValue(m_inputData.m_triggerL, m_inputReportsThisFrame, report.m_triggerL);
        HIDraGamepad_Private::AddAveragedValue(m_inputData.m_triggerR, m_inputReportsThisFrame, report.m_triggerR);
    }
} // namespace HIDra
#endif // HIDra_Gamepad