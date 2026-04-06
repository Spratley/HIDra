#include "HIDraGamepadHandler.h"

#if (HIDra_Platform == HIDra_Platform_Windows) && HIDra_Gamepad

#include <Windows.h>
// Windows.h must be included first
#include <cfgmgr32.h> // For any CM_ macros
#include <hidsdi.h>

#pragma comment(lib, "Cfgmgr32")
#pragma comment(lib, "hid.lib")

// TODO: Don't do this
#if HIDra_Debug
#define PRINT_WINDOWS_ERROR(ErrorMessage) printf(ErrorMessage " Windows Error Code: %i\n", GetLastError())
#define PRINT_ERROR(ErrorMessageFmt, ...) printf(ErrorMessageFmt "\n", __VA_ARGS__)
#else
#define PRINT_WINDOWS_ERROR(ErrorMessage)
#define PRINT_ERROR(ErrorMessageFmt, ...)
#endif // HIDra_Debug

namespace
{
    bool IsDeviceGamepad(HANDLE deviceHandle)
    {
        HIDD_ATTRIBUTES attributes;
        if (!HidD_GetAttributes(deviceHandle, &attributes))
        {
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            return false;
        }

        PHIDP_PREPARSED_DATA preparsedData;
        if (!HidD_GetPreparsedData(deviceHandle, &preparsedData))
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

    bool InitGamepadWindowsData(HANDLE deviceHandle,
                                HIDra::Vendor& outVendor,
                                HIDra::Product& outProduct,
                                HIDra::GamepadPlatformData& outWindowsData)
    {
        outWindowsData.m_openDeviceHandle = deviceHandle;
        outWindowsData.m_lastKnownDeviceHandle = deviceHandle;

        wchar_t serialNumberBuffer[256] = {};
        if (!HidD_GetSerialNumberString(outWindowsData.m_openDeviceHandle,
                                        serialNumberBuffer,
                                        sizeof(serialNumberBuffer)))
        {
            PRINT_WINDOWS_ERROR("HidD_GetSerialNumberString failed!");
            outWindowsData.m_deviceSerialNumber = L"";
            return false;
        }
        else
        {
            outWindowsData.m_deviceSerialNumber = serialNumberBuffer;
        }

        HIDD_ATTRIBUTES attributes;
        if (!HidD_GetAttributes(outWindowsData.m_openDeviceHandle, &attributes))
        {
            PRINT_WINDOWS_ERROR("HidD_GetAttributes failed!");
            return false;
        }

        PHIDP_PREPARSED_DATA preparsedData;
        if (!HidD_GetPreparsedData(outWindowsData.m_openDeviceHandle, &preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidD_GetPreparsedData failed!");
            return false;
        }

        HIDP_CAPS capabilities;
        if (!HidP_GetCaps(preparsedData, &capabilities))
        {
            PRINT_WINDOWS_ERROR("HidP_GetCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        outWindowsData.m_reportSize = capabilities.InputReportByteLength;
        outVendor = attributes.VendorID;
        outProduct = attributes.ProductID;

        // Button Capabilities
        HIDra::HIDra_UInt16 buttonCapabilitiesCount = capabilities.NumberInputButtonCaps;
        if (buttonCapabilitiesCount == 0)
        {
            PRINT_WINDOWS_ERROR("Gamepad has 0 buttons!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        outWindowsData.m_buttonCapabilities = new HIDP_BUTTON_CAPS[buttonCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS
            != HidP_GetButtonCaps(HidP_Input,
                                  static_cast<PHIDP_BUTTON_CAPS>(outWindowsData.m_buttonCapabilities),
                                  &buttonCapabilitiesCount,
                                  preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetButtonCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        // Value Capabilities
        outWindowsData.m_valueCapabilitiesCount = capabilities.NumberInputValueCaps;
        if (outWindowsData.m_valueCapabilitiesCount == 0)
        {
            PRINT_WINDOWS_ERROR("Gamepad has 0 axes!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        outWindowsData.m_valueCapabilities = new HIDP_VALUE_CAPS[outWindowsData.m_valueCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS
            != HidP_GetValueCaps(HidP_Input,
                                 static_cast<PHIDP_VALUE_CAPS>(outWindowsData.m_valueCapabilities),
                                 &outWindowsData.m_valueCapabilitiesCount,
                                 preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetValueCaps failed!");
            HidD_FreePreparsedData(preparsedData);
            return false;
        }

        outWindowsData.m_preparsedData = preparsedData;
        return true;
    }

    HIDra::Gamepad CreateGamepad(HANDLE deviceHandle, HIDra::GamepadID id)
    {
        HIDra::Vendor vendor = 0x0000;
        HIDra::Product product = 0x0000;
        HIDra::GamepadPlatformData windowsData;
        if (!InitGamepadWindowsData(deviceHandle, vendor, product, windowsData))
        {
            CloseHandle(deviceHandle);
            windowsData.m_openDeviceHandle = INVALID_HANDLE_VALUE;
        }
        return HIDra::Gamepad(vendor, product, id, std::move(windowsData));
    }
} // namespace

namespace HIDra
{
#if HIDra_GP_Multiple
    bool GamepadHandler::GatherAllGamepadsImpl()
    {
        GUID hidGuid;
        HidD_GetHidGuid(&hidGuid);

        ULONG deviceInterfaceListSize = 0;
        CONFIGRET cr = CM_Get_Device_Interface_List_Size(&deviceInterfaceListSize,
                                                         &hidGuid,
                                                         NULL,
                                                         CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
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

        PSTR deviceInterfaceList = new CHAR[deviceInterfaceListSize];
        ZeroMemory(deviceInterfaceList, deviceInterfaceListSize * sizeof(CHAR));

        cr = CM_Get_Device_Interface_List(&hidGuid,
                                          NULL,
                                          (PZZSTR)deviceInterfaceList,
                                          deviceInterfaceListSize,
                                          CM_GET_DEVICE_INTERFACE_LIST_PRESENT);
        if (cr != CR_SUCCESS)
        {
            PRINT_ERROR("Could not retrieve device interface list! Error %i", cr);
            delete[] deviceInterfaceList;
            return false;
        }

        HANDLE deviceHandle = INVALID_HANDLE_VALUE;
        for (PSTR currentInterface = deviceInterfaceList; *currentInterface;
             currentInterface += strlen(currentInterface) + 1)
        {
            deviceHandle = CreateFile((LPCSTR)currentInterface,
                                      /*GENERIC_READ | GENERIC_WRITE*/ 0,
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
                // TODO: Figure this out more
                m_gamepads.emplace_back(
                  CreateGamepad(deviceHandle, static_cast<HIDra::GamepadID::StorageType>(m_gamepads.size())));
                continue;
            }
            CloseHandle(deviceHandle);
        }

        delete[] deviceInterfaceList;
        return !m_gamepads.empty();
    }
#endif // HIDra_GP_Multiple
} // namespace HIDra

// Still don't do this
#undef PRINT_ERROR
#undef PRINT_WINDOWS_ERROR
#endif // (HIDra_Platform == HIDra_Platform_Windows) && HIDra_Gamepad