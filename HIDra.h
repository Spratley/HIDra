#pragma once
#include "HIDraEnums.h"
#include "Gamepad/HIDraGamepadManager.h"
#include "Keyboard/HIDraKeyboardManager.h"

namespace HIDra
{
    class Core : public PlatformCore
    {
    public:
        static Core& GetInstance() { return s_instance; }
        static Core const& GetInstanceConst() { return s_instance; }
    private:
        static Core s_instance;

    public:
        bool Init(PlatformCoreInitData& initData);
        void Flush();

#if HIDra_Keyboard
        KeyboardManager& GetKeyboardManager() { return m_keyboardManager; }
        KeyboardManager const& GetKeyboardManager() const { return m_keyboardManager; }
#endif // HIDra_Keyboard

#if HIDra_Gamepad
        GamepadManager& GetGamepadManager() { return m_gamepadManager; }
        GamepadManager const& GetGamepadManager() const { return m_gamepadManager; }

        float GetAxis(GamepadAxisID axis, GamepadID gamepadID) const;
        Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID) const;

        bool GetButton(GamepadButtonFlags button, GamepadID gamepadID) const;
        bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID) const;
        bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID) const;
#endif // HIDra_Gamepad

    private:
        GamepadManager m_gamepadManager;
        KeyboardManager m_keyboardManager;
    };
    
    // Non-member functions so end-user calls don't need to double-namespace HIDra::Core::Func()
    inline bool Init(PlatformCoreInitData& initData) { return Core::GetInstance().Init(initData); }
    inline void Flush() { return Core::GetInstance().Flush(); }

#if HIDra_Keyboard
    inline bool GetKey(KeyCode keyCode) { return Core::GetInstanceConst().GetKeyboardManager().GetKey(keyCode); }
    inline bool GetKeyDown(KeyCode keyCode) { return Core::GetInstanceConst().GetKeyboardManager().GetKeyDown(keyCode); }
    inline bool GetKeyUp(KeyCode keyCode) { return Core::GetInstanceConst().GetKeyboardManager().GetKeyUp(keyCode); }
#endif // HIDra_Keyboard

#if HIDra_Gamepad
    // Enable overrides to poll a specific ID only if the feature is enabled
#if HIDra_GP_Multiple
    inline float GetAxis(GamepadAxisID axis, GamepadID gamepadID) { return Core::GetInstanceConst().GetAxis(axis, gamepadID); }
    inline Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID) { return Core::GetInstanceConst().GetAxis2D(axis, gamepadID); }

    inline bool GetButton(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstanceConst().GetButton(button, gamepadID); }
    inline bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstanceConst().GetButtonDown(button, gamepadID); }
    inline bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID) { return Core::GetInstanceConst().GetButtonUp(button, gamepadID); }
#endif // HIDra_GP_Multiple

    // Enable overrides to poll generic gamepad only if the feature is enabled
#if HIDra_GP_Generic
    inline float GetAxis(GamepadAxisID axis) { return Core::GetInstanceConst().GetAxis(axis, GenericGamepadID); }
    inline Vec2f GetAxis2D(GamepadAxisID axis) { return Core::GetInstanceConst().GetAxis2D(axis, GenericGamepadID); }

    inline bool GetButton(GamepadButtonFlags button) { return Core::GetInstanceConst().GetButton(button, GenericGamepadID); }
    inline bool GetButtonDown(GamepadButtonFlags button) { return Core::GetInstanceConst().GetButtonDown(button, GenericGamepadID); }
    inline bool GetButtonUp(GamepadButtonFlags button) { return Core::GetInstanceConst().GetButtonUp(button, GenericGamepadID); }
#endif // HIDra_GP_Generic

#endif // HIDra_Gamepad
}