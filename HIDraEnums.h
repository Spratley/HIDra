#pragma once

#include "HIDraDefs.h"
#include "HIDraTypes.h"

namespace HIDra
{
    // Keyboard

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
    };
#endif // HIDra_Gamepad
}

