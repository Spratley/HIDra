#pragma once
#include "../../../HIDraDefs.h"

#include "../../../Gamepad/HIDraGamepad.h"

#include <string>
#include <vector>

#if HIDra_WebAssembly && HIDra_Gamepad
namespace HIDra
{
    class Gamepad_WASM : public GamepadBase
    {};

    using Gamepad = Gamepad_WASM;
} // namespace HIDra
#endif // HIDra_WebAssembly && HIDra_Gamepad