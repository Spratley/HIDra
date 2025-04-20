#include "HIDraGamepadDecoder_Windows.h"

#include "HIDraGamepad_Windows.h"

#define MAP_BUTTON_ID_FLAG(ButtonID, ButtonFlag) case ButtonID: heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | ButtonFlag); continue

namespace HIDra
{
    constexpr float uint16ToFloatDivisor = 1.0f / static_cast<float>(0xFFFF);
    // Remaps a UInt16 from range [0x0000 to 0xFFFF] to float [-1.0f to 1.0f]
    inline float UInt16ToSignedNormalizedFloat(HIDra_UInt16 value) { return (value * uint16ToFloatDivisor) * 2.0f - 1.0f; }

    // Nintendo Switch Pro Controller
    enum class NintendoSwitchProInputIDs
    {
        A = 1,
        B = 2,
        X = 3,
        Y = 4,

        BumperL = 5,
        BumperR = 6,

        TriggerL = 7,
        TriggerR = 8,
        
        Minus = 9,
        Plus = 10,

        StickL = 11,
        StickR = 12,

        Axis_StickLX = 48,
        Axis_StickLY = 49,
        Axis_StickRX = 51,
        Axis_StickRY = 52,

        Axis_DPad = 57
    };

    void GamepadDecoderNSP::Decode(GamepadReport_Windows const& report, GamepadInputData& outInputData)
    {
        GamepadButtonFlags heldButtonFlags = BID_NONE;

        bool triggerLHeld = false;
        bool triggerRHeld = false;

        for (HIDra_UInt16 heldButtonID : report.m_heldButtonIDs)
        {
            // Map input button IDs to readable gamepad button flags
            switch (static_cast<NintendoSwitchProInputIDs>(heldButtonID))
            {
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::A, BID_EAST);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::B, BID_SOUTH);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::X, BID_NORTH);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::Y, BID_WEST);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::BumperL, BID_BUMPER_L);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::BumperR, BID_BUMPER_R);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::Minus, BID_MINUS);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::Plus, BID_PLUS);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::StickL, BID_STICK_L);
                MAP_BUTTON_ID_FLAG(NintendoSwitchProInputIDs::StickR, BID_STICK_R);

                // Special Case
                // Triggers are read as buttons for the Nintendo Switch,
                // but we're mapping them to analogue inputs for compatability
            case NintendoSwitchProInputIDs::TriggerL:
                triggerLHeld = true;
                continue;
            case NintendoSwitchProInputIDs::TriggerR:
                triggerRHeld = true;
                continue;
            }
        }

        outInputData.SetTriggerL(triggerLHeld ? 1.0f : 0.0f);
        outInputData.SetTriggerR(triggerRHeld ? 1.0f : 0.0f);

        // Translate and Dispatch Axes
        for (GamepadReport_Windows::Axis axis : report.m_axes)
        {
            switch (static_cast<NintendoSwitchProInputIDs>(axis.m_id))
            {
            case NintendoSwitchProInputIDs::Axis_StickLX:
                outInputData.SetStickLX(UInt16ToSignedNormalizedFloat(axis.m_value));
                break;
            case NintendoSwitchProInputIDs::Axis_StickLY:
                outInputData.SetStickLY(-UInt16ToSignedNormalizedFloat(axis.m_value)); // Sticks are read inverted, flip the values on translation
                break;
            case NintendoSwitchProInputIDs::Axis_StickRX:
                outInputData.SetStickRX(UInt16ToSignedNormalizedFloat(axis.m_value));
                break;
            case NintendoSwitchProInputIDs::Axis_StickRY:
                outInputData.SetStickRY(-UInt16ToSignedNormalizedFloat(axis.m_value)); // Sticks are read inverted, flip the values on translation
                break;

            // The DPad on a Nintendo Switch is very weird
            // It's read as an axis, but the value represents a compound of which directions are held
            // The thing is- the values are clockwise around the DPad starting from the north one
            // So it's not like bit flags, it's just arbitrary values that represent direction combinations
            // Craziest part? 8 means you're holding none of them (upside_down_smiley_face)
            // NORTH == 0, NORTH_EAST == 1, EAST == 2, SOUTH_EAST == 3, SOUTH == 4, SOUTH_WEST == 5, WEST == 6, NORTH_WEST == 7, NONE == 8
            case NintendoSwitchProInputIDs::Axis_DPad:
                if (axis.m_value == 0 || axis.m_value == 1 || axis.m_value == 7) // Up
                {
                    heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_NORTH);
                }
                else if (axis.m_value == 4 || axis.m_value == 3 || axis.m_value == 5) // Down
                {
                    heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_SOUTH);
                }

                if (axis.m_value == 2 || axis.m_value == 1 || axis.m_value == 3) // Right
                {
                    heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_EAST);
                }
                else if (axis.m_value == 6 || axis.m_value == 5 || axis.m_value == 7) // Left
                {
                    heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_WEST);
                }
                break;
            }
        }

        // Dispatch buttons (Now that we've polled the DPad)
        outInputData.SetFlags(heldButtonFlags);
    }
}