#include "HIDraKeyboardManager.h"

namespace HIDra
{
    bool KeyboardManager::GetKey(KeyCode keyCode) const
    {
        return m_keyStateData.GetBit(keyCode);
    }

    bool KeyboardManager::GetKeyDown(KeyCode keyCode) const
    {
        return m_keyStateData.GetBit(keyCode) && m_keyStateSetThisFrame.GetBit(keyCode);
    }

    bool KeyboardManager::GetKeyUp(KeyCode keyCode) const
    {
        return (!m_keyStateData.GetBit(keyCode)) && m_keyStateSetThisFrame.GetBit(keyCode);
    }

    void KeyboardManager::SetKeyState(KeyCode keyCode, bool pressed)
    {
        if (GetKey(keyCode) != pressed)
        {
            m_keyStateData.SetBit(keyCode, pressed);
            m_keyStateSetThisFrame.SetBit(keyCode, true);
        }
    }

    void KeyboardManager::Flush()
    {
        m_keyStateSetThisFrame.ZeroMemory();
    }
}