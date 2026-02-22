#pragma once

#include "../../HIDraTypes.h"

#if HIDra_Platform == HIDra_WebAssembly
namespace HIDra
{
    class GamepadManager;

    struct PlatformCoreInitData_WASM
    {};
    using PlatformCoreInitData = PlatformCoreInitData_WASM;

    class PlatformCore_WASM
    {
    protected:
        bool PlatformInit(PlatformCoreInitData const& /*initData*/, GamepadManager& /*gamepadManager*/) { return true; }
    };
    using PlatformCore = PlatformCore_WASM;
} // namespace HIDra
#endif // HIDra_Platform == HIDra_WebAssembly