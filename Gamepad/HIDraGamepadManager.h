#pragma once

#include "../Platforms/HIDraPlatformInclude.inl"

#include <vector>

namespace HIDra
{
    // If there's more than 256 gamepads connected you've got bigger problems than ID collisions
    using GamepadID = unsigned char;
    const GamepadID GenericGamepad = 0xff;

    class GamepadManager
    {
    public:
        GamepadManager();

        void Flush();

        void FreeAllGamepads() { m_connectedGamepads.clear(); }

        bool HasGamepadsConnected() const { return m_connectedGamepads.size() > 0; }

        template <class Lambda>
        inline int FindGamepad(Lambda const& func);
        Gamepad& GetGamepad(GamepadID gamepadID);
        Gamepad const& GetGamepad(GamepadID gamepadID) const;

        std::vector<Gamepad>& GetGamepads() { return m_connectedGamepads; }

    private:
        // Use if you don't care about which gamepad the input is coming from
        // Processes button pressess from all connected gamepads
        // Axis values are the average of all read values
        Gamepad m_genericGamepad;
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
}