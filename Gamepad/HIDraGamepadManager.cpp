#include "HIDraGamepadManager.h"

#if HIDra_Gamepad

#if HIDra_Debug
#define PRINT_ERROR(message) printf(message "\n")
#else
#define PRINT_ERROR(message)
#endif // HIDra_Debug

namespace HIDra
{
    void GamepadManager::Flush()
    {
#if HIDra_GP_Generic
        m_genericGamepad.Flush();
#endif // HIDra_GP_Generic

        for (Gamepad& gamepad : m_connectedGamepads)
        {
            gamepad.Flush();
        }
    }

    // Gross macros to sort out which gamepads we should grab
    GamepadBase const* GamepadManager::GetGamepad(GamepadID gamepadID) const
    {
#if HIDra_GP_Generic
        if (gamepadID == GenericGamepadID)
        {
            return &m_genericGamepad;
        }
#endif // HIDra_GP_Generic

#if HIDra_GP_Multiple
        if (GamepadBase const* connectedGamepad = GetConnectedGamepad(gamepadID))
        {
            return connectedGamepad;
        }
#endif // HIDra_GP_Multiple

#if HIDra_GP_Generic
        return &m_genericGamepad;
#else
        return nullptr;
#endif // HIDra_GP_Generic
    }

    Gamepad const* GamepadManager::GetConnectedGamepad(GamepadID gamepadID) const
    {
#if HIDra_GP_Generic
        if (gamepadID == GenericGamepadID)
        {
            PRINT_ERROR("Attempting to access generic gamepad with GetConnectedGamepad(). Please use GetGenericGamepad() instead.");
            return nullptr;
        }
#endif // HIDra_GP_Generic

        if (gamepadID >= m_connectedGamepads.size())
        {
            PRINT_ERROR("Attempting to access a gamepad out of bounds!");
            return nullptr;
        }

        return &m_connectedGamepads[gamepadID];
    }

    Gamepad* GamepadManager::GetConnectedGamepad(GamepadID gamepadID)
    {
        return const_cast<Gamepad*>(const_cast<GamepadManager const*>(this)->GetConnectedGamepad(gamepadID));
    }
}
#endif // HIDra_Gamepad