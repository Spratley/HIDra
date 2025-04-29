#pragma once

#include "HIDraGamepad.h"

#if HIDra_Gamepad && HIDra_GP_Generic
namespace HIDra
{
    class GenericGamepad : public GamepadBase
    {
    public:
        GenericGamepad();

        void ReportInput(GamepadInputData const& inputReport);

        // I've only got the body in the header because this class is so small
        inline void Flush() override 
        { 
            GamepadBase::Flush(); 
            m_inputReportsThisFrame = 0; 
        }

    private:
        void AddAxisInput(float value, GamepadAxisID axis);
        void AddAxis2DInput(Vec2f value, GamepadAxisID axis);

    private:
        HIDra_UInt32 m_inputReportsThisFrame;
        HIDra_UInt32 m_axisInputReportsThisFrame[AID_COUNT];
    };
}
#endif // HIDra_Gamepad && HIDra_GP_Generic