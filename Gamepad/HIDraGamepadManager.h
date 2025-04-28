#pragma once
#include "../Platforms/HIDraPlatformInclude.inl"
#include "HIDraGenericGamepad.h"

#include <vector>

namespace HIDra
{
#if HIDra_Gamepad
    // If there's more than 256 gamepads connected you've got bigger problems than ID collisions
    using GamepadID = unsigned char;

#if HIDra_GP_Generic
    const GamepadID GenericGamepadID = 0xff;
#endif // HIDra_GP_Generic

    class GamepadManager
    {
    public:
        GamepadManager() = default;

        void FreeAllGamepads() { m_connectedGamepads.clear(); }

        void Flush();

        GamepadBase const* GetGamepad(GamepadID gamepadID) const;

        bool HasGamepadsConnected() const { return m_connectedGamepads.size() > 0; }
        template <class Lambda>
        inline int FindGamepad(Lambda const& func);
        Gamepad* GetConnectedGamepad(GamepadID gamepadID);
        Gamepad const* GetConnectedGamepad(GamepadID gamepadID) const;

        std::vector<Gamepad>& GetGamepads() { return m_connectedGamepads; }

#if HIDra_GP_Generic
        GenericGamepad& GetGenericGamepad() { return m_genericGamepad; }
        GenericGamepad const& GetGenericGamepad() const { return m_genericGamepad; }
#endif // HIDra_GP_Generic

    private:
#if HIDra_GP_Generic
        // Use if you don't care about which gamepad the input is coming from
        // Processes button pressess from all connected gamepads
        // Axis values are the average of all read values
        GenericGamepad m_genericGamepad;
#endif // HIDra_GP_Generic
        
        std::vector<Gamepad> m_connectedGamepads;
    };

    template <class Lambda>
    inline int GamepadManager::FindGamepad(Lambda const& func)
    {
        for (int i = 0; i < m_connectedGamepads.size(); i++)
        {
            if (func(m_connectedGamepads[i]))
            {
                return i;
            }
        }

        return -1;
    }
#else
    class GamepadManager {};
#endif // HIDra_Gamepad
}