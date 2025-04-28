#include "../HIDraDefs.h"
#if HIDra_Windows
#include "Windows/HIDraCore_Windows.h"
#include "Windows/Gamepad/HIDraGamepad_Windows.h"
#elif HIDra_Linux
#error HIDra has no support for Linux yet!
#elif HIDra_OSX
#error HIDra has no support for OSX yet!
#else
#error HIDra has no platform defined! Double check your HIDraDefs.h and make sure you have a platform selected!
#endif