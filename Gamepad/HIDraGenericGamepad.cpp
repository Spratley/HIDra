#include "../HIDraDefs.h"
#include "HIDraGenericGamepad.h"

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

        // Average axis reports
        Vec2f const newStickL = AddAveragedValue(m_inputData.GetStickL(), m_inputReportsThisFrame, inputReport.GetStickL());
        Vec2f const newStickR = AddAveragedValue(m_inputData.GetStickR(), m_inputReportsThisFrame, inputReport.GetStickR());
    
        m_inputData.SetStickL(newStickL);
        m_inputData.SetStickR(newStickR);

        float const newTriggerL = AddAveragedValue(m_inputData.GetTriggerL(), m_inputReportsThisFrame, inputReport.GetTriggerL());
        float const newTriggerR = AddAveragedValue(m_inputData.GetTriggerR(), m_inputReportsThisFrame, inputReport.GetTriggerR());

        m_inputData.SetTriggerL(newTriggerL);
        m_inputData.SetTriggerR(newTriggerR);

        m_inputReportsThisFrame++;
    }
}
#endif // HIDra_Gamepad && HIDra_GP_Generic