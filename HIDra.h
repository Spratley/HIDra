#pragma once
#include "Core/HIDraCore.h"
#include "HIDraEnums.h"

namespace HIDra
{
    // inline bool Init(PlatformCoreInitData& initData) { return Core::GetInstance().Init(initData); }
    inline void Flush() { return Core::GetInstance().Flush(); }

#if HIDra_Keyboard
    inline bool GetKey(KeyCode keyCode) { return Core::GetInstanceConst().GetKeyboardHandler().GetKey(keyCode); }
    inline bool GetKeyDown(KeyCode keyCode)
    {
        return Core::GetInstanceConst().GetKeyboardHandler().GetKeyDown(keyCode);
    }
    inline bool GetKeyUp(KeyCode keyCode) { return Core::GetInstanceConst().GetKeyboardHandler().GetKeyUp(keyCode); }
#endif // HIDra_Keyboard

#if HIDra_Gamepad
    // Enable overrides to poll a specific ID only if the feature is enabled
#if HIDra_GP_Multiple
    // TODO: Validate GamepadID
    inline float GetAxis(GamepadAxisID axis, GamepadID gamepadID)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGamepad(gamepadID)->GetAxis(axis);
    }
    inline Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGamepad(gamepadID)->GetAxis2D(axis);
    }

    inline bool GetButton(GamepadButtonFlags button, GamepadID gamepadID)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGamepad(gamepadID)->GetButton(button);
    }
    inline bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGamepad(gamepadID)->GetButtonDown(button);
    }
    inline bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGamepad(gamepadID)->GetButtonUp(button);
    }
#endif // HIDra_GP_Multiple

    // Enable overrides to poll generic gamepad only if the feature is enabled
#if HIDra_GP_Generic
    inline float GetAxis(GamepadAxisID axis)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGenericGamepad().GetAxis(axis);
    }
    inline Vec2f GetAxis2D(GamepadAxisID axis)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGenericGamepad().GetAxis2D(axis);
    }

    inline bool GetButton(GamepadButtonFlags button)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGenericGamepad().GetButton(button);
    }
    inline bool GetButtonDown(GamepadButtonFlags button)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGenericGamepad().GetButtonDown(button);
    }
    inline bool GetButtonUp(GamepadButtonFlags button)
    {
        return Core::GetInstanceConst().GetGamepadHandler().GetGenericGamepad().GetButtonUp(button);
    }
#endif // HIDra_GP_Generic

#endif // HIDra_Gamepad
} // namespace HIDra