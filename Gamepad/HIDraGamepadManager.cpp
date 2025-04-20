#include "HIDraGamepadManager.h"

#include "../HIDraDefs.h"

#include <iostream>

namespace HIDra
{
    GamepadManager::GamepadManager()
        : m_genericGamepad()
    {}

    void GamepadManager::Flush()
    {
        m_genericGamepad.Flush();
        for (Gamepad& gamepad : m_connectedGamepads)
        {
            gamepad.Flush();
        }
    }

    Gamepad const& GamepadManager::GetGamepad(GamepadID gamepadID) const
    {
        if (gamepadID == GenericGamepad)
        {
            return m_genericGamepad;
        }

#if HIDra_Debug
        if (gamepadID >= m_connectedGamepads.size())
        {
            printf("Attempting to access a gamepad out of bounds!");
            return m_genericGamepad;
        }
#endif // HIDra_Debug
        return m_connectedGamepads[gamepadID];
    }

    Gamepad& GamepadManager::GetGamepad(GamepadID gamepadID)
    {
        if (gamepadID == GenericGamepad)
        {
            return m_genericGamepad;
        }

#if HIDra_Debug
        if (gamepadID >= m_connectedGamepads.size())
        {
            printf("Attempting to access a gamepad out of bounds!");
            return m_genericGamepad;
        }
#endif // HIDra_Debug
        return m_connectedGamepads[gamepadID];
    }
}