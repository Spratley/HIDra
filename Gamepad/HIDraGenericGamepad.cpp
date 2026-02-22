#include "../Defs/HIDraDefs.h"
#include "HIDraGenericGamepad.h"

#include <cmath>

#if HIDra_Gamepad && HIDra_GP_Generic
// Local helper
template <class ValueType>
inline ValueType AddAveragedValue(ValueType const& previousAverage, HIDra::HIDra_UInt32 previousCount, ValueType const& value)
{
    return ((previousAverage * previousCount) + value) / (previousCount + 1);
}

namespace HIDra
{
    GenericGamepad::GenericGamepad()
        : m_inputReportsThisFrame(0)
        , m_axisInputReportsThisFrame{}
    {}

    void GenericGamepad::ReportInput(GamepadInputData const& inputReport)
    {
        if (m_inputReportsThisFrame == 0)
        {
            // If this is the first report this frame, just duplicate the data
            m_inputData.SetFlags(inputReport.GetFlags());
            m_inputData.SetStickL(inputReport.GetStickL());
            m_inputData.SetStickR(inputReport.GetStickR());
            m_inputData.SetTriggerL(inputReport.GetTriggerL());
            m_inputData.SetTriggerR(inputReport.GetTriggerR());
            m_inputReportsThisFrame++;
            return;
        }

        // Combine incoming flags with others reported this frame
        GamepadButtonFlags const frameTotalFlags = static_cast<GamepadButtonFlags>(m_inputData.GetFlags() & inputReport.GetFlags());
        m_inputData.SetFlags(frameTotalFlags);
        m_inputReportsThisFrame++;

        // Average axis reports
        AddAxisInput(inputReport.GetTriggerL(), AID_TRIGGER_L);
        AddAxisInput(inputReport.GetTriggerR(), AID_TRIGGER_R);
        AddAxis2DInput(inputReport.GetStickL(), AID_STICK_L);
        AddAxis2DInput(inputReport.GetStickR(), AID_STICK_R);
    }

    void GenericGamepad::AddAxisInput(float value, GamepadAxisID axis)
    {
        // Ignore values under the dead zone so the average better reflects the connected gamepads
        if (std::fabs(value) < AxisDeadZone)
        {
            return;
        }

        float currentValue = m_inputData.GetAxis(axis);
        float newValue = AddAveragedValue(currentValue, m_axisInputReportsThisFrame[axis], value);
        m_inputData.SetAxis(axis, newValue);
        m_axisInputReportsThisFrame[axis]++;
    }

    void GenericGamepad::AddAxis2DInput(Vec2f value, GamepadAxisID axis)
    {
        // Ignore values under the dead zone so the average better reflects the connected gamepads
        if (std::fabs(value.m_x) < AxisDeadZone && std::fabs(value.m_y) < AxisDeadZone)
        {
            return;
        }

        Vec2f currentValue = m_inputData.GetAxis2D(axis);
        Vec2f newValue = AddAveragedValue(currentValue, m_axisInputReportsThisFrame[axis], value);
        m_inputData.SetAxis(axis, newValue);
        m_axisInputReportsThisFrame[axis]++;
    }
}
#endif // HIDra_Gamepad && HIDra_GP_Generic