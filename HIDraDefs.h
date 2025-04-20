#pragma once

// These define what features are compiled
// Feel free to edit this to integrate it with your build setup

#define HIDra_Gamepad true
#define HIDra_SupportMultipleGamepad true

#define HIDra_Windows true

#ifdef _DEBUG
#define HIDra_Debug true
#else
#define HIDra_Debug false
#endif // _DEBUG