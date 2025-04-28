#include "../../HIDraDefs.h"
#if HIDra_Windows
#include "../../HIDra.h"

#include <cstdio>

#include <windows.h>
#include <cfgmgr32.h>
#include <hidusage.h>
#include <dbt.h>
#include <hidsdi.h>

#pragma comment(lib, "hid.lib")
#pragma comment(lib, "Cfgmgr32")

#if HIDra_Debug
#define PRINT_WINDOWS_ERROR(ErrorMessage) printf(ErrorMessage " Windows Error Code: %i\n", GetLastError())
#define PRINT_ERROR(ErrorMessageFmt, ...) printf(ErrorMessageFmt "\n", __VA_ARGS__)
#else
#define PRINT_WINDOWS_ERROR(ErrorMessage)
#define PRINT_ERROR(ErrorMessageFmt, ...)
#endif // HIDra_Debug

// Disable unreferenced parameter warnings
#pragma warning (disable : 4100)

// Hijack the windows message handler to filter input messages through HIDra
WNDPROC defaultWindowProcedure;
LRESULT CALLBACK WindowsRawInputMessageHandler(HWND windowHandle, UINT message, WPARAM wParam, LPARAM lParam)
{
    if (message == WM_INPUT)
    {
        // Ideally this data would be passed off to an input handler so we're not blocking the windows message
        // In my own testing a blank opengl app runs at 700 fps on my potato laptop without any magic handoff and async processing
        // Still it's a stretch goal, but not right now

        // Check data size
        UINT inputMessageSize;
        GetRawInputData((HRAWINPUT)lParam, RID_INPUT, NULL, &inputMessageSize, sizeof(RAWINPUTHEADER));
        LPBYTE messageBuffer = new BYTE[inputMessageSize];

        // Read data
        UINT expectedSize = inputMessageSize;
        if (expectedSize == GetRawInputData((HRAWINPUT)lParam, RID_INPUT, messageBuffer, &inputMessageSize, sizeof(RAWINPUTHEADER)))
        {
            RAWINPUT* rawData = (RAWINPUT*)messageBuffer;
            

            if (rawData->header.dwType == RIM_TYPEKEYBOARD)
            {
                // TODO: Keyboard Input
            }
#if HIDra_Gamepad
            else if (rawData->header.dwType == RIM_TYPEHID)
            {
                // Moved here temporarily to avoid C4189 when gamepads are disabled
                HIDra::Core& core = HIDra::Core::GetInstance();
                core.ProcessGamepadInput(rawData, core.GetGamepadManager());
            }
#endif // HIDra_Gamepad
        }
        else
        {
            PRINT_WINDOWS_ERROR("GetRawInputData failed to return correct size!");
        }
        delete[] messageBuffer;
    }
    else if (message == WM_DEVICECHANGE)
    {
        // TODO: Process device change messages
    }

    return CallWindowProc(defaultWindowProcedure, windowHandle, message, wParam, lParam);
}

namespace HIDra
{
    namespace Local
    {
        // Local helper functions
        std::wstring GetDeviceSerialNumber(DeviceHandle device)
        {
            UINT nameSize;
            GetRawInputDeviceInfo(device, RIDI_DEVICENAME, nullptr, &nameSize);

            wchar_t* deviceName = new wchar_t[nameSize];
            if (GetRawInputDeviceInfo(device, RIDI_DEVICENAME, deviceName, &nameSize) <= 0)
            {
                PRINT_WINDOWS_ERROR("Failed to get device name!");
                delete[] deviceName;
                return L"";
            }

            HANDLE deviceHandle = CreateFile(deviceName,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                nullptr,
                OPEN_EXISTING,
                0,
                nullptr);
            delete[] deviceName;

            if (deviceHandle == INVALID_HANDLE_VALUE)
            {
                PRINT_WINDOWS_ERROR("CreateFile failed!");
                return L"";
            }

            wchar_t serialNumberBuffer[256] = { 0 };
            if (!HidD_GetSerialNumberString(deviceHandle, serialNumberBuffer, sizeof(serialNumberBuffer)))
            {
                PRINT_WINDOWS_ERROR("HidD_GetSerialNumberString failed!");
                CloseHandle(deviceHandle);
                return L"";
            }
            CloseHandle(deviceHandle);
            return std::wstring(serialNumberBuffer);
        }

#if HIDra_Gamepad
        Gamepad* GetGamepadByHandle(DeviceHandle device, GamepadManager& gamepadManager)
        {
            int foundGamepadID = gamepadManager.FindGamepad([&device](Gamepad& gamepad)
                {
                    return device == gamepad.GetLastKnownDeviceHandle();
                });

            if (foundGamepadID != -1)
            {
                return gamepadManager.GetConnectedGamepad(static_cast<GamepadID>(foundGamepadID));
            }

            // Not found! Let's compare serial numbers...
            std::wstring serialNumber = GetDeviceSerialNumber(device);
            foundGamepadID = gamepadManager.FindGamepad([&device, &serialNumber](Gamepad& gamepad)
                {
                    return gamepad.GetSerialNumber().compare(serialNumber) == 0;
                });

            if (foundGamepadID != -1)
            {
                Gamepad* gamepad = gamepadManager.GetConnectedGamepad(static_cast<GamepadID>(foundGamepadID));
                gamepad->SetLastKnownDeviceHandle(device);
                return gamepad;
            }

            return nullptr;
        }
#endif // HIDra_Gamepad
    }

    bool PlatformCore_Windows::PlatformInit(PlatformCoreInitData const& initData, GamepadManager& gamepadManager)
    {
        HWND hWnd = (HWND)initData.m_mainWindowHandle;
        defaultWindowProcedure = (WNDPROC)(SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)WindowsRawInputMessageHandler));
    
        if (!SubscribeToInputMessages())
        {
            return false;
        }

#if HIDra_Gamepad
        if (!SubscribeToDeviceChanges(hWnd))
        {
            return false;
        }

        return GatherGamepads(gamepadManager);
#else
        
        return true;
#endif // HIDra_Gamepad
    }

#if HIDra_Gamepad
    void PlatformCore_Windows::ProcessGamepadInput(RawInputData rawInput, GamepadManager& gamepadManager)
    {
        RAWINPUT* rawInputData = static_cast<RAWINPUT*>(rawInput);

        if (!gamepadManager.HasGamepadsConnected())
        {
            return;
        }

        Gamepad* const gamepad = Local::GetGamepadByHandle(rawInputData->header.hDevice, gamepadManager);
        if (!gamepad)
        {
            // TODO: Queue unknown handles for registry...
            // It's probably better to do that via WM_DEVICECHANGE than this error, but once that's in ideally this error should never trigger
            PRINT_ERROR("Failed to find a gamepad corresponding with the input report handle!");
            return;
        }

        BYTE* report = rawInputData->data.hid.bRawData;
        HIDra_UInt32 reportSize = rawInputData->data.hid.dwCount * rawInputData->data.hid.dwSizeHid;

        // Parse Button Presses
        USAGE usages[64];
        ULONG usageLength = 64;

        PHIDP_BUTTON_CAPS buttonCapabilities = static_cast<PHIDP_BUTTON_CAPS>(gamepad->GetButtonCapabilities());
        PHIDP_PREPARSED_DATA preparsedData = static_cast<PHIDP_PREPARSED_DATA>(gamepad->GetPreparsedData());

        NTSTATUS buttonResult = HidP_GetUsages(HidP_Input, buttonCapabilities[0].UsagePage, 0, usages, &usageLength, preparsedData, (PCHAR)report, reportSize);
        if (buttonResult != HIDP_STATUS_SUCCESS)
        {
            PRINT_ERROR("HidpGetUsages failed! HID Error Result: %i", buttonResult);
            return;
        }

        // Build Gamepad Report
        GamepadReport_Windows inputReport;
        inputReport.m_heldButtonIDs.resize(usageLength);
        inputReport.m_axes.reserve(gamepad->GetPredictedAxisCount());

        memcpy(inputReport.m_heldButtonIDs.data(), usages, sizeof(HIDra_UInt16) * usageLength);

        // Parse Axis Values
        const HIDra_UInt32 valueCapabilitiesCount = gamepad->GetValueCapabilitiesCount();
        PHIDP_VALUE_CAPS valueCapabilities = static_cast<PHIDP_VALUE_CAPS>(gamepad->GetValueCapabilities());

        for (HIDra_UInt32 i = 0; i < valueCapabilitiesCount; i++)
        {
            ULONG value = 0;
            NTSTATUS result = HidP_GetUsageValue(HidP_Input, valueCapabilities[i].UsagePage, 0, valueCapabilities[i].NotRange.Usage, &value, preparsedData, (PCHAR)report, reportSize);
            if (result != HIDP_STATUS_SUCCESS)
            {
                continue;
            }
            inputReport.m_axes.push_back(GamepadReport_Windows::Axis(valueCapabilities[i].NotRange.Usage, static_cast<HIDra_UInt16>(value)));
        }

        gamepad->ReportInput(inputReport);

#if HIDra_GP_Generic
        // Send report to generic gamepad
        GamepadInputData const& gamepadInput = gamepad->GetInputData();
        gamepadManager.GetGenericGamepad().ReportInput(gamepadInput);
#endif // HIDra_GP_Generic

        return;
    }
#endif // HIDra_Gamepad

    bool PlatformCore_Windows::SubscribeToInputMessages()
    {
#if HIDra_Gamepad
        constexpr HIDra_UInt32 rawInputDeviceCount = 3;
#else
        constexpr HIDra_UInt32 rawInputDeviceCount = 2;
#endif // HIDra_Gamepad

        RAWINPUTDEVICE rid[rawInputDeviceCount];

        // Keyboard
        rid[0].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[0].usUsage = HID_USAGE_GENERIC_KEYBOARD;
        rid[0].dwFlags = 0;
        rid[0].hwndTarget = 0;

        // Mouse
        rid[1].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[1].usUsage = HID_USAGE_GENERIC_MOUSE;
        rid[1].dwFlags = 0;
        rid[1].hwndTarget = 0;

#if HIDra_Gamepad
        // Gamepad
        rid[2].usUsagePage = HID_USAGE_PAGE_GENERIC;
        rid[2].usUsage = HID_USAGE_GENERIC_GAMEPAD;
        rid[2].dwFlags = 0;
        rid[2].hwndTarget = 0;
#endif // HIDra_Gamepad
        
        if (RegisterRawInputDevices(rid, rawInputDeviceCount, sizeof(rid[0])) == FALSE)
        {
            PRINT_WINDOWS_ERROR("Failed to register RawInput devices!");
            return false;
        }

        return true;
    }

#if HIDra_Gamepad
    // TODO: Check if this is needed when running in only generic gamepad mode!
    // I don't think so
    bool PlatformCore_Windows::SubscribeToDeviceChanges(WindowHandle hWnd)
    {
        GUID hidGUID;
        HidD_GetHidGuid(&hidGUID);

        DEV_BROADCAST_DEVICEINTERFACE devBroadcastDeviceInterface;
        ZeroMemory(&devBroadcastDeviceInterface, sizeof(devBroadcastDeviceInterface));
        
        devBroadcastDeviceInterface.dbcc_size = sizeof(devBroadcastDeviceInterface);
        devBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        devBroadcastDeviceInterface.dbcc_classguid = hidGUID;

        HDEVNOTIFY deviceNotificationHandle = RegisterDeviceNotification((HWND)hWnd, &devBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!deviceNotificationHandle)
        {
            PRINT_WINDOWS_ERROR("Failed to register for device change notifications!");
            return false;
        }

        return true;
    }

    bool PlatformCore_Windows::IsDeviceGamepad(DeviceHandle device) const
    {
        HIDD_ATTRIBUTES attributes;
        if (!HidD_GetAttributes(device, &attributes))
        {
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            return false;
        }

        PHIDP_PREPARSED_DATA preparsedData;
        if (!HidD_GetPreparsedData(device, &preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidD_GetPreparsedData failed!");
            return false;
        }

        HIDP_CAPS capabilities;
        if (!HidP_GetCaps(preparsedData, &capabilities))
        {
            PRINT_WINDOWS_ERROR("HidP_GetCaps failed!");
            return false;
        }

        return capabilities.UsagePage == HID_USAGE_PAGE_GENERIC && capabilities.Usage == HID_USAGE_GENERIC_GAMEPAD;
    }

    bool PlatformCore_Windows::GatherGamepads(GamepadManager& gamepadManager)
    {
        gamepadManager.FreeAllGamepads();

        std::vector<Gamepad>& gamepads = gamepadManager.GetGamepads();

        GUID hidGuid;
        HidD_GetHidGuid(&hidGuid);

        ULONG deviceInterfaceListSize = 0;
        CONFIGRET cr = CM_Get_Device_Interface_List_Size(&deviceInterfaceListSize, &hidGuid, NULL, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
        if (cr != CR_SUCCESS)
        {
            PRINT_ERROR("Could not retrieve device interface list size! Error %i", cr);
            return false;
        }

        if (deviceInterfaceListSize == 0)
        {
            // No gamepad devices detected
            return true;
        }

        PWSTR deviceInterfaceList = new WCHAR[deviceInterfaceListSize];
        ZeroMemory(deviceInterfaceList, deviceInterfaceListSize * sizeof(WCHAR));

        cr = CM_Get_Device_Interface_List(&hidGuid, NULL, deviceInterfaceList, deviceInterfaceListSize, CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
        if (cr != CR_SUCCESS)
        {
            PRINT_ERROR("Could not retrieve device interface list! Error %i", cr);
            delete[] deviceInterfaceList;
            return false;
        }
        
        DeviceHandle deviceHandle = INVALID_HANDLE_VALUE;
        for (PWSTR currentInterface = deviceInterfaceList; *currentInterface; currentInterface += wcslen(currentInterface) + 1)
        {
            deviceHandle = CreateFile(currentInterface,
                GENERIC_READ | GENERIC_WRITE,
                FILE_SHARE_READ | FILE_SHARE_WRITE,
                NULL,
                OPEN_EXISTING,
                0,
                NULL);

            if (deviceHandle == INVALID_HANDLE_VALUE)
            {
                continue;
            }

            if (IsDeviceGamepad(deviceHandle))
            {
                gamepads.push_back(Gamepad(deviceHandle));
                continue;
            }
            CloseHandle(deviceHandle);
        }

        delete[] deviceInterfaceList;
        return gamepads.size() > 0;
    }
#endif // HIDra_Gamepad
}
#undef PRINT_ERROR
#undef PRINT_WINDOWS_ERROR
#endif // HIDra_Windows
