#include "HIDraGamepadDecoder_Windows.h"

#if (HIDra_Platform == HIDra_Platform_Windows) && HIDra_Gamepad
#include "../../../Gamepad/HIDraGamepad.h"
#include "../../../Gamepad/HIDraHIDDatabase.h"

#include <format>
#include <iostream>

// Very lazy thing to do here
#define MAP_BUTTON_ID_FLAG(ButtonID, ButtonFlag) case ButtonID: heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | ButtonFlag); continue
#define MAP_BUTTON_ID_RAW_BIT(ButtonID, PageIndex, RawBit) heldButtonFlags = (rawReport[PageIndex] & RawBit) ? static_cast<GamepadButtonFlags>(heldButtonFlags | ButtonID) : heldButtonFlags

namespace HIDra
{
    constexpr float uint16ToFloatDivisor = 1.0f / static_cast<float>(0xFFFF);
    constexpr float uint12ToFloatDivisor = 1.0f / static_cast<float>(0x0FFF);
    constexpr float uint8ToFloatDivisor = 1.0f / static_cast<float>(0x00FF);

    inline float UInt16ToUnsignedNormalizedFloat(HIDra_UInt16 value, float divisor = uint16ToFloatDivisor)
    {
        return (static_cast<float>(value) * divisor);
    }

    inline float UInt16ToSignedNormalizedFloat(HIDra_UInt16 value, float divisor = uint16ToFloatDivisor)
    {
        return UInt16ToUnsignedNormalizedFloat(value, divisor) * 2.0f - 1.0f;
    }

    // Nintendo Switch Pro Controller
    namespace NSP
    {
        // Hardcoded IDs sent by the NSP controller
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

        void Decode(GamepadReport_Windows const& report, GamepadInputData& outInputData)
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
                    case NintendoSwitchProInputIDs::TriggerL: triggerLHeld = true; continue;
                    case NintendoSwitchProInputIDs::TriggerR: triggerRHeld = true; continue;
                }
            }

            outInputData.m_triggerL = (triggerLHeld ? 1.0f : 0.0f);
            outInputData.m_triggerR = (triggerRHeld ? 1.0f : 0.0f);

            // Translate and Dispatch Axes
            Vec2f& stickL = outInputData.m_stickL;
            Vec2f& stickR = outInputData.m_stickR;
            for (GamepadReport_Windows::Axis axis : report.m_axes)
            {
                switch (static_cast<NintendoSwitchProInputIDs>(axis.m_id))
                {
                    case NintendoSwitchProInputIDs::Axis_StickLX:
                        stickL.m_x = UInt16ToSignedNormalizedFloat(axis.m_value);
                        break;
                    case NintendoSwitchProInputIDs::Axis_StickLY:
                        stickL.m_y = -UInt16ToSignedNormalizedFloat(
                          axis.m_value); // Sticks are read inverted, flip the values on translation
                        break;
                    case NintendoSwitchProInputIDs::Axis_StickRX:
                        stickR.m_x = UInt16ToSignedNormalizedFloat(axis.m_value);
                        break;
                    case NintendoSwitchProInputIDs::Axis_StickRY:
                        stickR.m_y = -UInt16ToSignedNormalizedFloat(
                          axis.m_value); // Sticks are read inverted, flip the values on translation
                        break;

                    // TODO: I realized that YES you can hypothetically hold UP and DoWN, or LEFT and RIGHT at the same
                    // time, so I need to come back and figure out what code is sent when ALL those combinations happen

                    // The DPad on a Nintendo Switch is very weird
                    // It's read as an axis, but the value represents a compound of which directions are held
                    // The thing is, the values are clockwise around the DPad starting from the north one
                    // So it's not like bit flags, it's just arbitrary values that represent direction combinations
                    // Craziest part? 8 means you're holding none of them (upside_down_smiley_face)
                    // NORTH == 0, NORTH_EAST == 1, EAST == 2, SOUTH_EAST == 3, SOUTH == 4, SOUTH_WEST == 5, WEST ==
                    // 6, NORTH_WEST == 7, NONE == 8
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
            outInputData.m_buttonFlags = heldButtonFlags;
        }

        void Decode(Byte const* rawReport, GamepadInputData& outInputData)
        {
            GamepadButtonFlags heldButtonFlags = BID_NONE;

            MAP_BUTTON_ID_RAW_BIT(BID_BUMPER_R, 3, 0b01000000);
            MAP_BUTTON_ID_RAW_BIT(BID_EAST, 3, 0b00001000);
            MAP_BUTTON_ID_RAW_BIT(BID_SOUTH, 3, 0b00000100);
            MAP_BUTTON_ID_RAW_BIT(BID_NORTH, 3, 0b00000010);
            MAP_BUTTON_ID_RAW_BIT(BID_WEST, 3, 0b00000001);

            MAP_BUTTON_ID_RAW_BIT(BID_HOME, 4, 0b00010000);
            MAP_BUTTON_ID_RAW_BIT(BID_STICK_L, 4, 0b00001000);
            MAP_BUTTON_ID_RAW_BIT(BID_STICK_R, 4, 0b00000100);
            MAP_BUTTON_ID_RAW_BIT(BID_PLUS, 4, 0b00000010);
            MAP_BUTTON_ID_RAW_BIT(BID_MINUS, 4, 0b00000001);

            MAP_BUTTON_ID_RAW_BIT(BID_BUMPER_L, 5, 0b01000000);
            MAP_BUTTON_ID_RAW_BIT(BID_DPAD_EAST, 5, 0b00000100);
            MAP_BUTTON_ID_RAW_BIT(BID_DPAD_SOUTH, 5, 0b00000001);
            MAP_BUTTON_ID_RAW_BIT(BID_DPAD_NORTH, 5, 0b00000010);
            MAP_BUTTON_ID_RAW_BIT(BID_DPAD_WEST, 5, 0b00001000);

            outInputData.m_buttonFlags = heldButtonFlags;

            outInputData.m_triggerR = (rawReport[3] & 0b1000000) ? 1.0f : 0.0f;
            outInputData.m_triggerL = (rawReport[5] & 0b1000000) ? 1.0f : 0.0f;

            HIDra_UInt16 stickLHorizontalRaw = rawReport[6] | ((rawReport[7] & 0xF) << 8);
            HIDra_UInt16 stickLVerticalRaw = (rawReport[7] >> 4) | (rawReport[8] << 4);
            outInputData.m_stickL.m_x = UInt16ToSignedNormalizedFloat(stickLHorizontalRaw, uint12ToFloatDivisor);
            outInputData.m_stickL.m_y = UInt16ToSignedNormalizedFloat(stickLVerticalRaw, uint12ToFloatDivisor);

            HIDra_UInt16 stickRHorizontalRaw = rawReport[9] | ((rawReport[10] & 0xF) << 8);
            HIDra_UInt16 stickRVerticalRaw = (rawReport[10] >> 4) | (rawReport[11] << 4);
            outInputData.m_stickR.m_x = UInt16ToSignedNormalizedFloat(stickRHorizontalRaw, uint12ToFloatDivisor);
            outInputData.m_stickR.m_y = UInt16ToSignedNormalizedFloat(stickRVerticalRaw, uint12ToFloatDivisor);
        }
    } // namespace NSP

    // DualSense (DualShock 5)
    namespace SONY_DS5
    {
        // Hardcoded IDs sent by the Dualsense controller
        enum class DualsenseInputIDs
        {
            Square = 1,
            Cross = 2,
            Circle = 3,
            Triangle = 4,

            BumperL = 5,
            BumperR = 6,

            TriggerL = 7,
            TriggerR = 8,

            Share = 9,
            Options = 10,

            StickL = 11,
            StickR = 12,

            PlayStation = 13,
            Touchpad = 14,

            Axis_StickLX = 48,
            Axis_StickLY = 49,
            Axis_StickRX = 50,
            Axis_StickRY = 53,

            Axis_TriggerL = 51,
            Axis_TriggerR = 52,

            Axis_DPad = 57,
        };

        void Decode(GamepadReport_Windows const& report, GamepadInputData& outInputData)
        {
            GamepadButtonFlags heldButtonFlags = BID_NONE;

            for (HIDra_UInt16 heldButtonID : report.m_heldButtonIDs)
            {
                // Map input button IDs to readable gamepad button flags
                switch (static_cast<DualsenseInputIDs>(heldButtonID))
                {
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Square, BID_EAST);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Cross, BID_SOUTH);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Circle, BID_WEST);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Triangle, BID_NORTH);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::BumperL, BID_BUMPER_L);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::BumperR, BID_BUMPER_R);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Share, BID_MINUS);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::Options, BID_PLUS);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::StickL, BID_STICK_L);
                    MAP_BUTTON_ID_FLAG(DualsenseInputIDs::StickR, BID_STICK_R);
                }
            }

            // Translate and Dispatch Axes
            Vec2f& stickL = outInputData.m_stickL;
            Vec2f& stickR = outInputData.m_stickR;
            for (GamepadReport_Windows::Axis axis : report.m_axes)
            {
                switch (static_cast<DualsenseInputIDs>(axis.m_id))
                {
                    case DualsenseInputIDs::Axis_StickLX:
                        stickL.m_x = UInt16ToSignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;
                    case DualsenseInputIDs::Axis_StickLY:
                        // Sticks are read inverted, flip the values on translation
                        stickL.m_y = -UInt16ToSignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;
                    case DualsenseInputIDs::Axis_StickRX:
                        stickR.m_x = UInt16ToSignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;
                    case DualsenseInputIDs::Axis_StickRY:
                        // Sticks are read inverted, flip the values on translation
                        stickR.m_y = -UInt16ToSignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;

                    case DualsenseInputIDs::Axis_TriggerL:
                        outInputData.m_triggerL = UInt16ToUnsignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;

                    case DualsenseInputIDs::Axis_TriggerR:
                        outInputData.m_triggerR = UInt16ToUnsignedNormalizedFloat(axis.m_value, uint8ToFloatDivisor);
                        break;

                    // Dualsense DPad sends its data as an axis value
                    // Starting with North = 0, the 8 cardinal directions count up clockwise
                    // 8 means no button is held
                    case DualsenseInputIDs::Axis_DPad:
                        if (axis.m_value == 0 || axis.m_value == 1 || axis.m_value == 7) // Up
                        {
                            heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_NORTH);
                        }
                        else if (axis.m_value == 3 || axis.m_value == 4 || axis.m_value == 5) // Down
                        {
                            heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_SOUTH);
                        }

                        if (axis.m_value == 1 || axis.m_value == 2 || axis.m_value == 3) // Right
                        {
                            heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_EAST);
                        }
                        else if (axis.m_value == 5 || axis.m_value == 6 || axis.m_value == 7) // Left
                        {
                            heldButtonFlags = static_cast<GamepadButtonFlags>(heldButtonFlags | BID_DPAD_WEST);
                        }
                        break;
                }
            }

            // Dispatch buttons (Now that we've polled the DPad)
            outInputData.m_buttonFlags = heldButtonFlags;
        }
    } // namespace SONY_DS5

    void DecodeWindowsReport(GamepadReport_Windows const& windowsReport, GamepadInputData& outInputData)
    {
        switch (windowsReport.m_vendorID)
        {
            case VID_NINTENDO:
                switch (windowsReport.m_productID)
                {
                    case PID_NINTENDO_SWITCH_PRO_CONTROLLER: NSP::Decode(windowsReport, outInputData); return;
                }
                break;
            case VID_SONY:
                switch (windowsReport.m_productID)
                {
                    // TODO: Parse Sony controllers
                    case PID_SONY_DUALSENSE: SONY_DS5::Decode(windowsReport, outInputData); return;
                    default:                 break;
                }
                break;
            default: break;
        }

        // TODO: Add error report here
        // Report is from unknown vendor ID
        // Is it possible to have some sort of fallback so we can still TRY and read the data?
    } // namespace HIDra

    void DecodeRawInputReport(Byte const* rawReport, Vendor vendor, Product product, GamepadInputData& outInputData)
    {
        switch (vendor)
        {
            case VID_NINTENDO:
                switch (product)
                {
                    case PID_NINTENDO_SWITCH_PRO_CONTROLLER: NSP::Decode(rawReport, outInputData); return;
                }
                break;
            default: break;
        }
    }

} // namespace HIDra
#endif // (HIDra_Platform == HIDra_Platform_Windows) && HIDra_Gamepad