#include "HIDraCore.h"

#if HIDra_Platform == HIDra_Platform_WebAssembly

namespace HIDra
{
    bool Core::Init(Core_PlatformInitData const& initData) { return true; }
} // namespace HIDra

#endif // HIDra_Platform == HIDra_Platform_WebAssembly