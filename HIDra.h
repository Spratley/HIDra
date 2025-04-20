#pragma once
#include "HIDraTypes.h"
#include "HIDraEnums.h"
#include "HIDraDefs.h"
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

        GamepadManager& GetGamepadManager() { return m_gamepadManager; }

        float GetAxis(GamepadAxisID axis, GamepadID gamepadID);
        Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID);

        bool GetButton(GamepadButtonFlags button, GamepadID gamepadID);
        bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID);
        bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID);

    private:
        GamepadManager m_gamepadManager;
    };

    inline bool Init(PlatformCoreInitData& initData) { return Core::GetInstance().Init(initData); }
    inline void Flush() { return Core::GetInstance().Flush(); }

    inline float GetAxis(GamepadAxisID axis, GamepadID gamepadID = GenericGamepad) { return Core::GetInstance().GetAxis(axis, gamepadID); }
    inline Vec2f GetAxis2D(GamepadAxisID axis, GamepadID gamepadID = GenericGamepad) { return Core::GetInstance().GetAxis2D(axis, gamepadID); }

    inline bool GetButton(GamepadButtonFlags button, GamepadID gamepadID = GenericGamepad) { return Core::GetInstance().GetButton(button, gamepadID); }
    inline bool GetButtonDown(GamepadButtonFlags button, GamepadID gamepadID = GenericGamepad) { return Core::GetInstance().GetButtonDown(button, gamepadID); }
    inline bool GetButtonUp(GamepadButtonFlags button, GamepadID gamepadID = GenericGamepad) { return Core::GetInstance().GetButtonUp(button, gamepadID); }
}