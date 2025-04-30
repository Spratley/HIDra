#pragma once

#include "HIDraTypes.h"

namespace HIDra
{
    // Keyboard
#if HIDra_Keyboard
    enum KeyCode : HIDra_UInt32
    {
         KEYCODE_INVALID = 0xFFFFFFFF

         // Numeric - Keep in order
        , KEYCODE_0 = 0
        , KEYCODE_1
        , KEYCODE_2
        , KEYCODE_3
        , KEYCODE_4
        , KEYCODE_5
        , KEYCODE_6
        , KEYCODE_7
        , KEYCODE_8
        , KEYCODE_9

         // Alphabet - Keep in order
        , KEYCODE_A
        , KEYCODE_B
        , KEYCODE_C
        , KEYCODE_D
        , KEYCODE_E
        , KEYCODE_F
        , KEYCODE_G
        , KEYCODE_H
        , KEYCODE_I
        , KEYCODE_J
        , KEYCODE_K
        , KEYCODE_L
        , KEYCODE_M
        , KEYCODE_N
        , KEYCODE_O
        , KEYCODE_P
        , KEYCODE_Q
        , KEYCODE_R
        , KEYCODE_S
        , KEYCODE_T
        , KEYCODE_U
        , KEYCODE_V
        , KEYCODE_W
        , KEYCODE_X
        , KEYCODE_Y
        , KEYCODE_Z

        // All other keys
        , KEYCODE_ESC

        , KEYCODE_F1
        , KEYCODE_F2
        , KEYCODE_F3
        , KEYCODE_F4
        , KEYCODE_F5
        , KEYCODE_F6
        , KEYCODE_F7
        , KEYCODE_F8
        , KEYCODE_F9
        , KEYCODE_F10
        , KEYCODE_F11
        , KEYCODE_F12

        , KEYCODE_SHIFT_L
        , KEYCODE_SHIFT_R
        , KEYCODE_CTRL_L
        , KEYCODE_CTRL_R
        , KEYCODE_ALT_L
        , KEYCODE_ALT_R

        , KEYCODE_SPACEBAR
        , KEYCODE_BACKSPACE
        , KEYCODE_CAPSLOCK
        , KEYCODE_ENTER
        
        , KEYCODE_TILDE
        , KEYCODE_PLUS
        , KEYCODE_MINUS
        , KEYCODE_INSERT
        , KEYCODE_DELETE

        , KEYCODE_OPENBRACE
        , KEYCODE_CLOSEBRACE

        , KEYCODE_BACKSLASH
        , KEYCODE_FORWARDSLASH
        
        , KEYCODE_SEMICOLON
        , KEYCODE_APOSTROPHE
        , KEYCODE_COMMA
        , KEYCODE_PERIOD

        , KEYCODE_ARROW_UP
        , KEYCODE_ARROW_RIGHT
        , KEYCODE_ARROW_DOWN
        , KEYCODE_ARROW_LEFT

        , KEYCODE_UNKNOWN
        , KEYCODE_COUNT
    };
#endif // HIDra_Keyboard

    // Mouse

    // Gamepad
#if HIDra_Gamepad
    enum GamepadButtonFlags : HIDra_UInt16
    {
          BID_NONE = 0

        , BID_EAST = 1 << 0
        , BID_SOUTH = 1 << 1
        , BID_WEST = 1 << 2
        , BID_NORTH = 1 << 3

        , BID_DPAD_EAST = 1 << 4
        , BID_DPAD_SOUTH = 1 << 5
        , BID_DPAD_WEST = 1 << 6
        , BID_DPAD_NORTH = 1 << 7

        , BID_BUMPER_R = 1 << 8
        , BID_BUMPER_L = 1 << 9

        , BID_STICK_R = 1 << 10
        , BID_STICK_L = 1 << 11

        , BID_PLUS = 1 << 12
        , BID_MINUS = 1 << 13
    };

    enum GamepadAxisID : HIDra_UInt8
    {
          AID_STICK_R
        , AID_STICK_L

        , AID_TRIGGER_R
        , AID_TRIGGER_L

        , AID_COUNT
    };
#endif // HIDra_Gamepad
}

