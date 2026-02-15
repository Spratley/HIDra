#pragma once

// These define what features are compiled
// Feel free to edit this to integrate it with your build setup

// -=-= Platform Core =-=-
#define HIDra_Windows 1
#define HIDra_WebAssembly 0
// #define HIDra_Linux 0    <HIDra has no Linux Support>
// #define HIDra_OSX 0      <HIDra has no OSX Support>

#if !(HIDra_Windows ^ HIDra_WebAssembly ^ HIDra_Linux ^ HIDra_OSX)
#error HIDra has multiple or no platforms selected! Please only define one as your build target.
#endif

// -=-= Feature Toggles =-=-
// Gamepad
#define HIDra_Gamepad 1

#if HIDra_Gamepad
    // What gamepad support is needed?
    #define HIDra_GP_Generic 1      // Gathers input reports from all connected controllers in a single all-purpose place. Ideal for single player or menus.
    #define HIDra_GP_Multiple 0     // Allows individual controllers to be polled by ID. Ideal for multi-player.
#else
    // Disable all gamepad features
    #define HIDra_GP_Generic 0
    #define HIDra_GP_Multiple 0
#endif // HIDra_Gamepad

#if HIDra_Gamepad && !HIDra_GP_Generic && !HIDra_GP_Multiple
#error You need at least one kind of gamepad support (facepalm)
#endif // HIDra_Gamepad && !HIDra_SupportGenericGAmepad && !HIDra_SupportMultipleGampead

// Keyboard
#define HIDra_Keyboard 1

// Mouse
#define HIDra_Mouse 0

#if !HIDra_Gamepad && !HIDra_Keyboard && !HIDra_Mouse
#error You're using HIDra but nothing is enabled? Bruh
#endif // !HIDra_Gamepad && !HIDra_Keyboard

// -=-= Debugging =-=-
#ifdef _DEBUG
#define HIDra_Debug 1
#else
#define HIDra_Debug 0
#endif // _DEBUG