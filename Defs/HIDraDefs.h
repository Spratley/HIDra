#pragma once

#define HIDra_Disable 0x00
#define HIDra_Enable 0x01

// -=-= Platforms =-=-
#define HIDra_Platform_Windows 0x00
#define HIDra_Platform_WebAssembly 0x01
// Note: HIDra doesn't have Linux support yet
#define HIDra_Platform_Linux 0x02
// Note: HIDra doesn't have OSX support yet
#define HIDra_Platform_OSX 0x03

// Controls what features are compiled
// To customize your build of HIDra, add the following file somewhere in your project
// Make sure to add its location to your Additional Include Directories so you don't have to edit everywhere it's used
// Define any values you need, look through the included HIDra_UserDefs.h.template to see what can be configured
#include "HIDra_UserDefs.h"

// Fallbacks in case the user has not defined something
// Enable all features and disable debugging by default
// Target windows

// -=-= Platform =-=-
#ifndef HIDra_Platform
#define HIDra_Platform HIDra_Platform_Windows
#endif // ndef HIDra_Platform

// -=-= Feature Toggles =-=-
// Gamepad
#ifndef HIDra_Gamepad
#define HIDra_Gamepad HIDra_Enable
#endif // ndef HIDra_Gamepad

#ifndef HIDra_GP_Generic
#define HIDra_GP_Generic HIDra_Enable
#endif // ndef HIDra_GP_Generic

#ifndef HIDra_GP_Multiple
#define HIDra_GP_Multiple HIDra_Enable
#endif // ndef HIDra_GP_Multiple

// Keyboard
#ifndef HIDra_Keyboard
#define HIDra_Keyboard HIDra_Enable
#endif // ndef HIDra_Keyboard

// Mouse
#ifndef HIDra_Mouse
#define HIDra_Mouse HIDra_Enable
#endif // ndef HIDra_Mouse

// -=-= Debugging =-=-
#ifndef HIDra_Debug
#define HIDra_Debug HIDra_Disable
#endif // ndef HIDra_Debug

// Configuration Validation
#if !HIDra_Gamepad && !HIDra_Keyboard && !HIDra_Mouse
#error You're using HIDra but nothing is enabled? Bruh
#endif // !HIDra_Gamepad && !HIDra_Keyboard && !HIDra_Mouse

#if HIDra_Gamepad && !HIDra_GP_Generic && !HIDra_GP_Multiple
#error You need at least one kind of gamepad support (facepalm)
#endif // HIDra_Gamepad && !HIDra_GP_Generic && !HIDra_GP_Multiple