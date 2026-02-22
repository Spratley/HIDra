#include "HIDraKeyCodeTranslator_Windows.h"

#if HIDra_Platform == HIDra_Windows
#include <windows.h>
// MUST come after windows.h
#include <WinUser.h>

#define MAP_VK_KEY(VK, KEYCODE) case VK: return KEYCODE

namespace HIDra
{
    KeyCode GetKeycodeFromWindowsVK(HIDra_UInt16 windowsKeyCode, HIDra_UInt16 flags)
    {
        // Handle alphabet characters first. Windows VK uses the uppercase ASCII values for alphabet and numeric
        // characters. Thankfully, ASCII is in series and we can do some math to translate them, so long as our KEYCODE
        // enum is also in alphabetic/numeric order.
        if (windowsKeyCode >= 'A' && windowsKeyCode <= 'Z')
        {
            return static_cast<KeyCode>(KEYCODE_A + (windowsKeyCode - 'A'));
        }

        if (windowsKeyCode >= '0' && windowsKeyCode <= '9')
        {
            return static_cast<KeyCode>(KEYCODE_0 + (windowsKeyCode - '0'));
        }

        if (windowsKeyCode >= VK_F1 && windowsKeyCode <= VK_F12)
        {
            return static_cast<KeyCode>(KEYCODE_F1 + (windowsKeyCode - VK_F1));
        }

        // Big switch to handle all other keycodes :(
        switch (windowsKeyCode)
        {
            MAP_VK_KEY(VK_ESCAPE, KEYCODE_ESC);
            MAP_VK_KEY(VK_SPACE, KEYCODE_SPACEBAR);
            MAP_VK_KEY(VK_BACK, KEYCODE_BACKSPACE);
            MAP_VK_KEY(VK_CAPITAL, KEYCODE_CAPSLOCK);
            MAP_VK_KEY(VK_RETURN, KEYCODE_ENTER);
            MAP_VK_KEY(VK_INSERT, KEYCODE_INSERT);
            MAP_VK_KEY(VK_DELETE, KEYCODE_DELETE);

            MAP_VK_KEY(VK_OEM_PLUS, KEYCODE_PLUS);
            MAP_VK_KEY(VK_OEM_MINUS, KEYCODE_MINUS);
            MAP_VK_KEY(VK_OEM_COMMA, KEYCODE_COMMA);
            MAP_VK_KEY(VK_OEM_PERIOD, KEYCODE_PERIOD);
            MAP_VK_KEY(VK_OEM_3, KEYCODE_TILDE);
            MAP_VK_KEY(VK_OEM_4, KEYCODE_OPENBRACE);
            MAP_VK_KEY(VK_OEM_6, KEYCODE_CLOSEBRACE);
            MAP_VK_KEY(VK_OEM_5, KEYCODE_BACKSLASH);
            MAP_VK_KEY(VK_OEM_2, KEYCODE_FORWARDSLASH);
            MAP_VK_KEY(VK_OEM_1, KEYCODE_SEMICOLON);
            MAP_VK_KEY(VK_OEM_7, KEYCODE_APOSTROPHE);

            MAP_VK_KEY(VK_UP, KEYCODE_ARROW_UP);
            MAP_VK_KEY(VK_RIGHT, KEYCODE_ARROW_RIGHT);
            MAP_VK_KEY(VK_DOWN, KEYCODE_ARROW_DOWN);
            MAP_VK_KEY(VK_LEFT, KEYCODE_ARROW_LEFT);

            MAP_VK_KEY(
              VK_SHIFT,
              KEYCODE_SHIFT_L); // KEYCODE_SHIFT_R is currently unused, right shift is assumed to be left shift :(

            // Special case for CTRL and ALT
            // Determine left vs right with flags
            // RAWINPUT sets RI_KEY_E0 to 1 if the key is the right version
            case VK_CONTROL: return ((flags & RI_KEY_E0) != 0) ? KEYCODE_CTRL_R : KEYCODE_CTRL_L;
            case VK_MENU: // Alt
                return ((flags & RI_KEY_E0) != 0) ? KEYCODE_ALT_R : KEYCODE_ALT_L;

            default: return KEYCODE_UNKNOWN;
        }
    }
} // namespace HIDra
#endif // HIDra_Platform == HIDra_Windows