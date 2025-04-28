#pragma once

#if HIDra_Gamepad
namespace HIDra
{
    // Nintendo
    const Vendor VID_NINTENDO = 0x057E;
    const Product PID_NINTENDO_JOYCON_L = 0x2006;
    const Product PID_NINTENDO_JOYCON_R = 0x2007;
    const Product PID_NINTENDO_SWITCH_PRO_CONTROLLER = 0x2009;

    // Sony
    const Vendor VID_SONY = 0x054C;
    const Product PID_SONY_DUALSHOCK_4 = 0x09CC;
}
#endif // HIDra_Gamepad