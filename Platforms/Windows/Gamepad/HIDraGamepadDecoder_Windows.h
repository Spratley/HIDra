#pragma once

#if HIDra_Windows && HIDra_Gamepad
namespace HIDra
{
    struct GamepadReport_Windows;
    struct GamepadInputData;

    // Nintendo Switch Pro Controller
    class GamepadDecoderNSP
    {
    public:
        static void Decode(GamepadReport_Windows const& report, GamepadInputData& outInputData);
    };
}
#endif // HIDra_Windows && HIDra_Gamepad