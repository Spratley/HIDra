#pragma once

// Harcoded product and vendor list
// Very much a WIP as I find devices to test with
#if HIDra_Gamepad
namespace HIDra
{
    // Nintendo
    constexpr Vendor VID_NINTENDO = 0x057E;
    constexpr Product PID_NINTENDO_JOYCON_L = 0x2006;
    constexpr Product PID_NINTENDO_JOYCON_R = 0x2007;
    constexpr Product PID_NINTENDO_SWITCH_PRO_CONTROLLER = 0x2009;

    // Sony
    constexpr Vendor VID_SONY = 0x054C;
    constexpr Product PID_SONY_DUALSHOCK_4 = 0x09CC;
    constexpr Product PID_SONY_DUALSENSE = 0x0CE6;
}
#endif // HIDra_Gamepad