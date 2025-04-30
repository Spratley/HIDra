#pragma once

#include "../HIDraEnums.h"

namespace HIDra
{
#if HIDra_Keyboard
    class KeyboardManager
    {
    public:
        bool GetKey(KeyCode keyCode) const;
        bool GetKeyDown(KeyCode keyCode) const;
        bool GetKeyUp(KeyCode keyCode) const;

        void SetKeyState(KeyCode keyCode, bool pressed);

        void Flush();

    private:
        // Calculate at compile time how many bytes are needed to store a unique bit for each possible key
        // The amount of bytes required is equal to the number of keys divided by 8, rounded up
        // By adding 7 to the value of KEYCODE_COUNT, we offset the remainder by at most 1 byte
        // If said remainder would be 0, the 7 is not enough to add an extra empty byte to the total
        // Therefore ((KEYCODE_COUNT + 7) / 8) gives us the minimum required amount of bytes to store all our keys
        static constexpr HIDra_UInt32 RequiredBytes = ((KEYCODE_COUNT + 7) / 8);

        BigBitfield<RequiredBytes> m_keyStateData;
        BigBitfield<RequiredBytes> m_keyStateSetThisFrame;
    };
#else
    class KeyboardManager {};
#endif // HIDra_Keyboard
}