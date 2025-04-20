#pragma once

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