#include "HIDra.h"

namespace HIDra
{
    Core Core::s_instance;

    bool Core::Init(PlatformCoreInitData& initData)
    {
        if (!PlatformCore::PlatformInit(initData, m_gamepadManager))
        {
            return false;
        }
        return true;
    }

    void Core::Flush()
    {
        m_gamepadManager.Flush();
    }

    float Core::GetAxis(GamepadAxisID axis, GamepadID gamepadID)
    {
        Gamepad const& gamepad = m_gamepadManager.GetGamepad(gamepadID);
        switch (axis)
        {
        case GamepadAxisID::AID_TRIGGER_L:
            return gamepad.GetInputData().GetTriggerL();
        case GamepadAxisID::AID_TRIGGER_R:
            return gamepad.GetInputData().GetTriggerR();
        }
#if HIDra_Debug
        printf("Attempting to read a non-1D axis with GetAxis()!");
#endif // HIDra_Debug
        return 0.0f;
    }

    Vec2f Core::GetAxis2D(GamepadAxisID axis, GamepadID gamepadID)
    {
        Gamepad const& gamepad = m_gamepadManager.GetGamepad(gamepadID);
        switch (axis)
        {
        case GamepadAxisID::AID_STICK_L:
            return gamepad.GetInputData().GetStickL();
        case GamepadAxisID::AID_STICK_R:
            return gamepad.GetInputData().GetStickR();
        }
#if HIDra_Debug
        printf("Attempting to read a non-2D axis with GetAxis2D()!");
#endif // HIDra_Debug
        return Vec2f();
    }

    bool Core::GetButton(GamepadButtonFlags button, GamepadID gamepadID)
    {
        Gamepad const& gamepad = m_gamepadManager.GetGamepad(gamepadID);
        GamepadInputData const& inputData = gamepad.GetInputData();
        return inputData.GetFlag(button);
    }
    
    bool Core::GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID)
    {
        Gamepad const& gamepad = m_gamepadManager.GetGamepad(gamepadID);
        GamepadInputData const& inputData = gamepad.GetInputData();
        return inputData.GetFlag(button) && inputData.WasFlagSetThisFrame(button);
    }
    
    bool Core::GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID)
    {
        Gamepad const& gamepad = m_gamepadManager.GetGamepad(gamepadID);
        GamepadInputData const& inputData = gamepad.GetInputData();
        return (!inputData.GetFlag(button)) && inputData.WasFlagSetThisFrame(button);
    }
}