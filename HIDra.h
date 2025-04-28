#pragma once
#include "HIDraEnums.h"
#include "Gamepad/HIDraGamepadManager.h"

namespace HIDra
{
    class Core : public PlatformCore
    {
    public:
        static Core& GetInstance() { return s_instance; }
    private:
        static Core s_instance;

    public:
        bool Init(PlatformCoreInitData& initData);
        void Flush();

#if HIDra_Gamepad
        GamepadManager& GetGamepadManager() { return m_gamepadManager; }

        float GetAxis(GamepadAxisID axis, GamepadID gamepadID);
        Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID);

        bool GetButton(GamepadButtonFlags button, GamepadID gamepadID);
        bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID);
        bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID);
#endif // HIDra_Gamepad

    private:
        GamepadManager m_gamepadManager;
    };
    
    // Non-member functions so end-user calls don't need to double-namespace HIDra::Core::Func()
    inline bool Init(PlatformCoreInitData& initData) { return Core::GetInstance().Init(initData); }
    inline void Flush() { return Core::GetInstance().Flush(); }

#if HIDra_Gamepad

    // Enable overrides to poll a specific ID only if the feature is enabled
#if HIDra_GP_Multiple
    inline float GetAxis(GamepadAxisID axis, GamepadID gamepadID) { return Core::GetInstance().GetAxis(axis, gamepadID); }
    inline Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID) { return Core::GetInstance().GetAxis2D(axis, gamepadID); }

    inline bool GetButton(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstance().GetButton(button, gamepadID); }
    inline bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstance().GetButtonDown(button, gamepadID); }
    inline bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstance().GetButtonUp(button, gamepadID); }
#endif // HIDra_GP_Multiple

    // Enable overrides to poll generic gamepad only if the feature is enabled
#if HIDra_GP_Generic
    inline float GetAxis(GamepadAxisID axis) { return Core::GetInstance().GetAxis(axis, GenericGamepadID); }
    inline Vec2f GetAxis2D(GamepadAxisID axis) { return Core::GetInstance().GetAxis2D(axis, GenericGamepadID); }

    inline bool GetButton(GamepadButtonFlags button) { return Core::GetInstance().GetButton(button, GenericGamepadID); }
    inline bool GetButtonDown(GamepadButtonFlags button) { return Core::GetInstance().GetButtonDown(button, GenericGamepadID); }
    inline bool GetButtonUp(GamepadButtonFlags button) { return Core::GetInstance().GetButtonUp(button, GenericGamepadID); }
#endif // HIDra_GP_Generic

#endif // HIDra_Gamepad
}