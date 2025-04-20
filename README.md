# HIDra (Pronounced Hydra)
A simple C++ Human Interface Device input system for real-time software.

I can't guarantee this is good- I'm not an input system programmer after all.

HIDra is a WIP, and currently only supports gamepads (No mouse or keyboard). Generic gamepad polling without supplying a connection index is next to be added, so stay tuned. (Until then, just add a 0 to the end of your HIDra::Get___() calls)

HIDra supports Windows right now using RawInput. The next goal is to get XInput support for gamepads that work better with it (i.e. Xbox Controllers).

Personally I don't intend to support Linux or Mac any time soon since I don't use either but who knows!

Please read the license before using HIDra in your own project, it's still totally free and open source, but I've just added some attribution clauses because I crave attention.

## Quickstart Guide for VS Nerds
First things first- import the files into your project hierarchy. If you're using precompiled headers, make sure to exclude any HIDra .cpp files from it.

Out of the box, HIDra supports multiple unique gamepad input (For local multi-player). This and more are configurable in HIDraDefs.h, so feel free to edit that to better match your project needs.

To get HIDra running, you just need to include HIDra.h wherever you'll need it, and then call HIDra::Init() in your boot sequence, and HIDra::Flush() in your update loop, and that's it! Note: For Windows, HIDra::Init() requires a HWND to your main window, and that's on you to figure out.

Now you're able to poll input from your connected gamepad 

## Known Issues
🎮 **HIDra won't recognize gamepads connected after the program launches.**\
Coming Soon! WM_DEVICECHANGE has not been implemented, and so HIDra can't detect when controllers disconnect and reconnect in order to properly categorize their input.

🎮 **HIDra doesn't support polling all controllers in general.**\
Again, Coming Soon! Currently, when receiving input from a controller HIDra will only notify the associated Gamepad object. The intention is though, for the system to also provide that input data to the GenericGamepad. GenericGamepad can be polled by excluding the controller ID from any HIDra::Get___() calls, and will react to incoming button presses from any controller. GenericGamepad will also use the average of all the axis information passed in.