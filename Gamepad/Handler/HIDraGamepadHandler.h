#pragma once
#include "../../Defs/HIDraDefs.h"
#include "../HIDraGamepad.h"

#include <vector>

namespace HIDra
{
#if HIDra_Gamepad
#if HIDra_GP_Generic
    constexpr GamepadID GenericGamepadID = 0x00;
#endif // HIDra_GP_Generic

    class GamepadHandler
    {
    public:
        GamepadHandler();

        void Flush()
        {
            ForEachGamepad([](Gamepad& gamepad) { gamepad.Flush(); });
        }
        void ReportInput(GamepadID gamepadID, GamepadInputData const& inputReport);

#if HIDra_GP_Multiple
        bool GatherAllGamepads();
        void FreeAllGamepads()
        {
            m_gamepads.clear();
        }
#endif // HIDra_GP_Multiple

        Gamepad* GetGamepad(GamepadID gamepadID);
        Gamepad const* GetGamepad(GamepadID gamepadID) const;
        bool HasGamepadsConnected() const { return !m_gamepads.empty(); }

        template <typename Functor>
        inline constexpr void ForEachGamepad(Functor const& functor)
        {
            for (Gamepad& gamepad : m_gamepads)
            {
                functor(gamepad);
            }
        }

        template <typename Functor>
        inline GamepadID FindGamepad(Functor const& findFunctor) const
        {
            for (Gamepad const& gamepad : m_gamepads)
            {
                if (findFunctor(gamepad))
                {
                    return gamepad.GetID();
                }
            }
            return GamepadID::InvalidGamepadID;
        }

#if HIDra_GP_Generic
        // Generic Gamepad should always exist
        Gamepad& GetGenericGamepad() { return *GetGamepad(GenericGamepadID); };
        Gamepad const& GetGenericGamepad() const { return *GetGamepad(GenericGamepadID); }
#endif // HIDRA_GP_Generic

    private:
#pragma region PlatformSpecific
#if HIDra_GP_Multiple
        bool GatherAllGamepadsImpl();
#endif // HIDra_GP_Multiple
#pragma endregion

    private:
        // Vector is used because it's VERY likely the end-user only has a handful of controllers connected
        // If for some reason it becomes industry standard for several dozen controllers to be connected at once,
        // This should be replaced with an unordered_map
        std::vector<Gamepad> m_gamepads;
        // TODO: Implement disconnecting gamepads and persistent IDs
        // Right now, the system is unstable with gamepad IDs
        // If a new device is connected, all IDs are up for grabs and HIDra_GP_Multiple frankly won't work
        // GamepadHandler should remember which device ID was which GamepadID, and re-pair them as they come back
        // That way GamepadID is persistent
    };
#else
    class GamepadHandler
    {};
#endif // HIDra_Gamepad
} // namespace HIDra