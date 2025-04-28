# HIDra (Pronounced Hydra) 
A simple C++ Human Interface Device input system for real-time software. <img src="https://spratley.github.io/Images/HIDra_Logo.png" alt="HIDra logo" width="300" align=right>

I can't guarantee this is good- I'm not an input system programmer after all.

HIDra is a WIP, and currently only supports gamepads (No mouse or keyboard).

HIDra supports Windows right now using RawInput. The next goal is to get XInput support for gamepads that work better with it (i.e. Xbox Controllers).

Personally I don't intend to support Linux or Mac any time soon since I don't use either but who knows!

Please read the license before using HIDra in your own project, it's still totally free and open source, but I've just added some attribution clauses because I crave attention.

## Quickstart Guide for VS Nerds
First things first- import the files into your project hierarchy. If you're using precompiled headers, make sure to exclude any HIDra .cpp files from it.

Out of the box, HIDra supports multiple unique gamepad input (For local multi-player). This and more are configurable in HIDraDefs.h, so feel free to edit that to better match your project needs.

To get HIDra running, you just need to include HIDra.h wherever you'll need it, and then call HIDra::Init() in your boot sequence, and HIDra::Flush() in your update loop, and that's it! Note: For Windows, HIDra::Init() requires a HWND to your main window, and that's on you to figure out.

Now you're able to poll input from your connected gamepads by calling the various Get functions in found in the global scope of HIDra::. By passing in an ID value, you can poll a specific controller's input data. If HIDra_GP_Generic is enabled in HIDraDefs.cpp, you can poll from the generic gamepad, which tracks input from all connected gamepads at once. A button pressed on any controller will press it on the generic controller, and the axis values will be the summed average of all the connected gamepads.

## Known Issues
🎮 **HIDra won't recognize gamepads connected after the program launches.**\
Coming Soon! WM_DEVICECHANGE has not been implemented, and so HIDra can't detect when controllers disconnect and reconnect in order to properly categorize their input.

## Changelog
0.0 - Initial Push of HIDra\
0.1 (Apr-27th-2025) - Added generic gamepad polling
