#include "HIDraGamepad.h"
#if HIDra_Gamepad

namespace
{
    template <class ValueType>
    inline ValueType AddAveragedValue(ValueType const& previousAverage,
                                      HIDra::HIDra_UInt32 previousCount,
                                      ValueType const& value)
    {
        return ((previousAverage * previousCount) + value) / (previousCount + 1);
    }
} // namespace

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
            return;
        }

        // Raise additional flags for any buttons changed by this report
        m_buttonFlagsSetThisFrame = static_cast<GamepadButtonFlags>(
          m_buttonFlagsSetThisFrame | (m_inputData.m_buttonFlags ^ report.m_buttonFlags));
        m_inputData.m_buttonFlags = report.m_buttonFlags;

        AddAveragedValue(m_inputData.m_stickL, m_inputReportsThisFrame, report.m_stickL);
        AddAveragedValue(m_inputData.m_stickR, m_inputReportsThisFrame, report.m_stickR);
        AddAveragedValue(m_inputData.m_triggerL, m_inputReportsThisFrame, report.m_triggerL);
        AddAveragedValue(m_inputData.m_triggerR, m_inputReportsThisFrame, report.m_triggerR);
    }
} // namespace HIDra
#endif // HIDra_Gamepad