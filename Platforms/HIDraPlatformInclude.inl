#include "../Defs/HIDraDefs.h"
#if HIDra_Platform == HIDra_Platform_Windows
#include "Windows/HIDraCore_Windows.h"
#include "Windows/Gamepad/HIDraGamepad_Windows.h"
#elif HIDra_Platform == HIDra_Platform_WebAssembly
#include "WASM/HIDraCore_WASM.h"
#include "WASM/Gamepad/HIDraGamepad_WASM.h"
#elif HIDra_Platform == HIDra_Platform_Linux
#error HIDra has no support for Linux yet!
#elif HIDra_Platform == HIDra_Platform_OSX
#error HIDra has no support for OSX yet!
#else
#error HIDra has no platform defined! Double check your HIDraDefs.h and make sure you have a platform selected!
#endif