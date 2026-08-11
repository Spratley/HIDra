#include "HIDraCore.h"

// TODO: Sort and add preprocessor guards to filter out gamepad and keyboard data

#if HIDra_Platform == HIDra_Platform_WebAssembly

#include <emscripten.h>
#include <emscripten/html5.h>

#include <algorithm>
#include <array>
#include <iostream> // DO NOT SUBMIT

#include "../Gamepad/HIDraGamepad.h"

// TODO: Move keyboard locals to a util.inl?
#define HIDra_EM_UTF8_TO_KEYCODE_MAPPING \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit0", HIDra::KEYCODE_0 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit1", HIDra::KEYCODE_1 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit2", HIDra::KEYCODE_2 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit3", HIDra::KEYCODE_3 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit4", HIDra::KEYCODE_4 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit5", HIDra::KEYCODE_5 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit6", HIDra::KEYCODE_6 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit7", HIDra::KEYCODE_7 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit8", HIDra::KEYCODE_8 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Digit9", HIDra::KEYCODE_9 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyA", HIDra::KEYCODE_A ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyB", HIDra::KEYCODE_B ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyC", HIDra::KEYCODE_C ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyD", HIDra::KEYCODE_D ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyE", HIDra::KEYCODE_E ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyF", HIDra::KEYCODE_F ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyG", HIDra::KEYCODE_G ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyH", HIDra::KEYCODE_H ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyI", HIDra::KEYCODE_I ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyJ", HIDra::KEYCODE_J ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyK", HIDra::KEYCODE_K ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyL", HIDra::KEYCODE_L ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyM", HIDra::KEYCODE_M ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyN", HIDra::KEYCODE_N ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyO", HIDra::KEYCODE_O ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyP", HIDra::KEYCODE_P ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyQ", HIDra::KEYCODE_Q ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyR", HIDra::KEYCODE_R ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyS", HIDra::KEYCODE_S ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyT", HIDra::KEYCODE_T ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyU", HIDra::KEYCODE_U ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyV", HIDra::KEYCODE_V ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyW", HIDra::KEYCODE_W ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyX", HIDra::KEYCODE_X ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyY", HIDra::KEYCODE_Y ) \
HIDra_EM_UTF8_TO_KEYCODE_X("KeyZ", HIDra::KEYCODE_Z ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Escape", HIDra::KEYCODE_ESC ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F1", HIDra::KEYCODE_F1 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F2", HIDra::KEYCODE_F2 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F3", HIDra::KEYCODE_F3 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F4", HIDra::KEYCODE_F4 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F5", HIDra::KEYCODE_F5 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F6", HIDra::KEYCODE_F6 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F7", HIDra::KEYCODE_F7 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F8", HIDra::KEYCODE_F8 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F9", HIDra::KEYCODE_F9 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F10", HIDra::KEYCODE_F10 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F11", HIDra::KEYCODE_F11 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("F12", HIDra::KEYCODE_F12 ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ShiftLeft", HIDra::KEYCODE_SHIFT_L ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ShiftRight", HIDra::KEYCODE_SHIFT_R ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ControlLeft", HIDra::KEYCODE_CTRL_L ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ControlRight", HIDra::KEYCODE_CTRL_R ) \
HIDra_EM_UTF8_TO_KEYCODE_X("AltLeft", HIDra::KEYCODE_ALT_L ) \
HIDra_EM_UTF8_TO_KEYCODE_X("AltRight", HIDra::KEYCODE_ALT_R ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Space", HIDra::KEYCODE_SPACEBAR ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Backspace", HIDra::KEYCODE_BACKSPACE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("CapsLock", HIDra::KEYCODE_CAPSLOCK ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Enter", HIDra::KEYCODE_ENTER ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Backquote", HIDra::KEYCODE_TILDE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Equal", HIDra::KEYCODE_PLUS ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Minus", HIDra::KEYCODE_MINUS ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Insert", HIDra::KEYCODE_INSERT ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Delete", HIDra::KEYCODE_DELETE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("BracketLeft", HIDra::KEYCODE_OPENBRACE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("BracketRight", HIDra::KEYCODE_CLOSEBRACE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Backslash", HIDra::KEYCODE_BACKSLASH ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Slash", HIDra::KEYCODE_FORWARDSLASH ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Semicolon", HIDra::KEYCODE_SEMICOLON ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Quote", HIDra::KEYCODE_APOSTROPHE ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Comma", HIDra::KEYCODE_COMMA ) \
HIDra_EM_UTF8_TO_KEYCODE_X("Period", HIDra::KEYCODE_PERIOD ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ArrowUp", HIDra::KEYCODE_ARROW_UP ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ArrowRight", HIDra::KEYCODE_ARROW_RIGHT ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ArrowDown", HIDra::KEYCODE_ARROW_DOWN ) \
HIDra_EM_UTF8_TO_KEYCODE_X("ArrowLeft", HIDra::KEYCODE_ARROW_LEFT )

namespace
{
    constexpr HIDra::HIDra_UInt32 HashUTF8(char const* utf8Str)
    {
        HIDra::HIDra_UInt32 hash = 0x811C9DC5;
        constexpr HIDra::HIDra_UInt32 prime = 0x1000193;
        while (*utf8Str)
        {
            hash ^= static_cast<HIDra::HIDra_UInt32>(*utf8Str++);
            hash *= prime;
        }
        return hash;
    }

    // Static Validations
    constexpr bool ValidateEMUTF8HashCollision()
    {
        struct EMUTF8KeycodePair
        {
            HIDra::HIDra_UInt32 utf8CodeHash;
            HIDra::KeyCode hidraKeyCode;
        };

#define HIDra_EM_UTF8_TO_KEYCODE_X(utf8Str, keyCode) { HashUTF8(utf8Str), keyCode },
        const EMUTF8KeycodePair utf8KeycodeMapping[] = {
            HIDra_EM_UTF8_TO_KEYCODE_MAPPING{ 0, HIDra::KEYCODE_UNKNOWN }
        };
#undef HIDra_EM_UTF8_TO_KEYCODE_X

        const int keycodeMappingCount = sizeof(utf8KeycodeMapping) / sizeof(EMUTF8KeycodePair);
        for (int i = 0; i < keycodeMappingCount - 1; ++i)
        {
            for (int j = i + 1; j < keycodeMappingCount; ++j)
            {
                if (utf8KeycodeMapping[i].utf8CodeHash == utf8KeycodeMapping[j].utf8CodeHash)
                {
                    return false;
                }
            }
        }
        return true;
    }
    static_assert(ValidateEMUTF8HashCollision(),
                  "Collision detected in EM_UTF8 strings! Figure out a different hash function.");

    HIDra::KeyCode GetKeyCodeFromEMUTF8(EM_UTF8 const* eventString)
    {
        HIDra::HIDra_UInt32 eventStringHash = HashUTF8(eventString);

#define HIDra_EM_UTF8_TO_KEYCODE_X(utf8Str, keyCode) case HashUTF8(utf8Str): return keyCode;
        switch (eventStringHash)
        {
            HIDra_EM_UTF8_TO_KEYCODE_MAPPING default : return HIDra::KEYCODE_UNKNOWN;
        }
#undef HIDra_EM_UTF8_TO_KEYCODE_X
    }
#undef HIDra_EM_UTF8_TO_KEYCODE_MAPPING

    bool ProcessEmscriptenKeyboardEvent(int eventType, EmscriptenKeyboardEvent const* event, void* hidraCorePtr)
    {
        if (event->repeat)
        {
            // Discard repeat events, I don't care how many times you tell me you're holding the button
            return false;
        }

        // Technically it's unsafe to assume anything other than KEYUP is KEYDOWN
        // The only other valid event type is KEYPRESS which is also a KEYDOWN, and we're not listening for that anyway
        bool const isKeyDown = eventType != EMSCRIPTEN_EVENT_KEYUP;
        HIDra::KeyCode const keyCode = GetKeyCodeFromEMUTF8(event->code);
        HIDra::Core& core = *static_cast<HIDra::Core*>(hidraCorePtr);
        core.GetKeyboardHandler().SetKeyState(keyCode, isKeyDown);
        return true; // TODO: Allow F5 and other browser-specific buttons to passthrough?
    }

    // GAMEPAD

    void PrintGamepadEvent(EmscriptenGamepadEvent const* event)
    {
        std::cout << "\tNum Axes: " << event->numAxes << "\n\tNum Buttons: " << event->numButtons << "\n\t Axes:\n";

        for (int i = 0; i < event->numAxes; ++i)
        {
            std::cout << "\t\t" << event->axis[i] << "\n";
        }

        std::cout << "\tAnalog Buttons:\n";

        for (int i = 0; i < event->numButtons; ++i)
        {
            std::cout << "\t\t" << event->analogButton[i] << "\n";
        }

        std::cout << "\tDigital Buttons:\n";

        for (int i = 0; i < event->numButtons; ++i)
        {
            std::cout << "\t\t" << event->digitalButton[i] << "\n";
        }

        std::cout << "\tConnected: " << event->connected << "\n\tIndex: " << event->index << "\n\tID: " << event->id
                  << "\n\tMapping: " << event->mapping << "\n";
    }

    HIDra::GamepadID ProcessEmscriptenGamepadInput(EmscriptenGamepadEvent const& inputEvent,
                                                   HIDra::GamepadInputData& outInputReport)
    {
        if (std::strcmp(inputEvent.mapping, "") == 0)
        {
            // Non-standard controller detected! For now don't support it
            // If I find a non-standard controller in my collection, I'll think about adding it
            return HIDra::GamepadID::InvalidGamepadID;
        }

        // Stick Axes
        outInputReport.m_stickL.m_x = static_cast<float>(inputEvent.axis[0]);
        outInputReport.m_stickL.m_y = static_cast<float>(-inputEvent.axis[1]);
        outInputReport.m_stickR.m_x = static_cast<float>(inputEvent.axis[2]);
        outInputReport.m_stickR.m_y = static_cast<float>(-inputEvent.axis[3]);

        // Triggers
        outInputReport.m_triggerL = static_cast<float>(inputEvent.analogButton[6]);
        outInputReport.m_triggerR = static_cast<float>(inputEvent.analogButton[7]);

        // Buttons
        HIDra::GamepadButtonFlags heldButtonFlags = HIDra::BID_NONE;
#define MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(ButtonIndex, Flag) if (inputEvent.digitalButton[ButtonIndex]) { heldButtonFlags = static_cast<HIDra::GamepadButtonFlags>(heldButtonFlags | HIDra::Flag); }
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(0, BID_SOUTH);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(1, BID_EAST);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(2, BID_WEST);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(3, BID_NORTH);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(4, BID_BUMPER_L);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(5, BID_BUMPER_R);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(8, BID_MINUS);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(9, BID_PLUS);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(10, BID_STICK_L);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(11, BID_STICK_R);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(12, BID_DPAD_NORTH);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(13, BID_DPAD_SOUTH);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(14, BID_DPAD_WEST);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(15, BID_DPAD_EAST);
        MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG(16, BID_HOME);
#undef MAP_DIGITAL_BUTTON_TO_BUTTON_FLAG
        outInputReport.m_buttonFlags = heldButtonFlags;

        // TODO: Get gamepad ID
        return 0;
    }

    bool ProcessEmscriptenGamepadEvent(int eventType, EmscriptenGamepadEvent const* event, void* hidraCorePtr)
    {
        if (eventType == EMSCRIPTEN_EVENT_GAMEPADCONNECTED)
            std::cout << "Gamepad Connected";
        else
            std::cout << "Gamepad Disconnected";

        // PrintGamepadEvent(event);
        return true;
    }

} // namespace

namespace HIDra
{
    bool Core::Init(Core_PlatformInitData const& initData)
    {
        emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, ProcessEmscriptenKeyboardEvent);
        emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, this, false, ProcessEmscriptenKeyboardEvent);

        emscripten_set_gamepadconnected_callback(this, false, ProcessEmscriptenGamepadEvent);
        // emscripten_set_gamepaddisconnected_callback(this, false, ProcessEmscriptenGamepadEvent);

        return true;
    }

    void Core::PostFlush()
    {
        EMSCRIPTEN_RESULT sampleGamepadResult = emscripten_sample_gamepad_data();
        if (sampleGamepadResult != EMSCRIPTEN_RESULT_SUCCESS)
        {
            // Gamepads not supported on this browser
            // We've already thrown this error earlier, so no need to spam
            return;
        }

        GamepadHandler& gamepadHandler = GetGamepadHandler();
        int const numGamepads = emscripten_get_num_gamepads();
        for (int i = 0; i < numGamepads; ++i)
        {
            EmscriptenGamepadEvent gamepadStatus;
            EMSCRIPTEN_RESULT result = emscripten_get_gamepad_status(i, &gamepadStatus);
            if (result != EMSCRIPTEN_RESULT_SUCCESS || !gamepadStatus.connected)
            {
                continue;
            }

            GamepadInputData inputReport;
            GamepadID gamepadID = ProcessEmscriptenGamepadInput(gamepadStatus, inputReport);
            if (gamepadID.IsValid())
            {
                gamepadHandler.ReportInput(gamepadID, inputReport);
            }
        }
    }
} // namespace HIDra

#endif // HIDra_Platform == HIDra_Platform_WebAssembly