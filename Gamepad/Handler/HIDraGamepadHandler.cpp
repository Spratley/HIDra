#include "HIDraGamepadHandler.h"

#if HIDra_Gamepad

#if HIDra_Debug
#define PRINT_ERROR(message) printf(message "\n")
#else
#define PRINT_ERROR(message)
#endif // HIDra_Debug

namespace HIDra
{
    GamepadHandler::GamepadHandler()
    {
#if HIDra_GP_Generic
        // Add generic gamepad to slot 0
        m_gamepads.emplace_back(Gamepad());
#endif // HIDra_GP_Generic
    }

    void GamepadHandler::ReportInput(GamepadID gamepadID, GamepadInputData const& inputData)
    {
        if (Gamepad* gamepad = GetGamepad(gamepadID))
        {
            gamepad->Report(inputData);
        }

#if HIDra_GP_Generic
        if (gamepadID != GenericGamepadID)
        {
            GetGenericGamepad().Report(inputData);
        }
#endif // HIDra_GP_Generic
    }

#if HIDra_GP_Multiple
    bool GamepadHandler::GatherAllGamepads()
    {
        FreeAllGamepads();
#if HIDra_GP_Generic
        // Add generic gamepad to slot 0
        m_gamepads.emplace_back(Gamepad());
#endif // HIDra_GP_Generic
        return GatherAllGamepadsImpl();
    }
#endif // HIDra_GP_Multiple

    Gamepad* GamepadHandler::GetGamepad(GamepadID gamepadID)
    {
        return const_cast<Gamepad*>(std::as_const(*this).GetGamepad(gamepadID));
    }

    Gamepad const* GamepadHandler::GetGamepad(GamepadID gamepadID) const
    {
        if (gamepadID >= m_gamepads.size())
        {
            PRINT_ERROR("Attempting to access a gamepad out of bounds!");
            return nullptr;
        }
        return &m_gamepads[gamepadID];
    }
} // namespace HIDra
#endif // HIDra_Gamepad