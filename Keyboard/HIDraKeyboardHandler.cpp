#include "HIDraKeyboardHandler.h"

#if HIDra_Keyboard
namespace HIDra
{
    bool KeyboardHandler::GetKey(KeyCode keyCode) const { return m_keyStateData.GetBit(keyCode); }

    bool KeyboardHandler::GetKeyDown(KeyCode keyCode) const
    {
        return m_keyStateData.GetBit(keyCode) && m_keyStateSetThisFrame.GetBit(keyCode);
    }

    bool KeyboardHandler::GetKeyUp(KeyCode keyCode) const
    {
        return (!m_keyStateData.GetBit(keyCode)) && m_keyStateSetThisFrame.GetBit(keyCode);
    }

    void KeyboardHandler::SetKeyState(KeyCode keyCode, bool pressed)
    {
        if (GetKey(keyCode) != pressed)
        {
            m_keyStateData.SetBit(keyCode, pressed);
            m_keyStateSetThisFrame.SetBit(keyCode, true);
        }
    }

    void KeyboardHandler::Flush() { m_keyStateSetThisFrame.ZeroMemory(); }
} // namespace HIDra
#endif // HIDra_Keyboard