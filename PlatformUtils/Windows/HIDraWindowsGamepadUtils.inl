// This is not the best structure
// These should probably be sorted and moved to a better home than an inl file

// TODO: This is an old include path, it should be moved
#include "../../Platforms/Windows/Gamepad/HIDraGamepadDecoder_Windows.h"

#if HIDra_Gamepad
namespace
{
    // TODO: Check if this is needed when running in only generic gamepad mode! (I don't think so)
    bool SubscribeToDeviceChanges(HWND hWnd)
    {
        GUID hidGUID;
        HidD_GetHidGuid(&hidGUID);

        DEV_BROADCAST_DEVICEINTERFACE devBroadcastDeviceInterface;
        ZeroMemory(&devBroadcastDeviceInterface, sizeof(devBroadcastDeviceInterface));

        devBroadcastDeviceInterface.dbcc_size = sizeof(devBroadcastDeviceInterface);
        devBroadcastDeviceInterface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
        devBroadcastDeviceInterface.dbcc_classguid = hidGUID;

        HDEVNOTIFY deviceNotificationHandle =
          RegisterDeviceNotification(hWnd, &devBroadcastDeviceInterface, DEVICE_NOTIFY_WINDOW_HANDLE);
        if (!deviceNotificationHandle)
        {
            PRINT_WINDOWS_ERROR("Failed to register for device change notifications!");
            return false;
        }

        return true;
    }

    // TODO: Refactor
    std::wstring GetDeviceSerialNumber(HANDLE rawDeviceHandle)
    {
        UINT nameSize;
        GetRawInputDeviceInfo(rawDeviceHandle, RIDI_DEVICENAME, nullptr, &nameSize);

        char* deviceName = new char[nameSize];
        if (GetRawInputDeviceInfo(rawDeviceHandle, RIDI_DEVICENAME, deviceName, &nameSize) <= 0)
        {
            PRINT_WINDOWS_ERROR("Failed to get device name!");
            delete[] deviceName;
            return L"";
        }

        HANDLE hidDeviceHandle =
          CreateFile((LPCSTR)deviceName, 0, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
        delete[] deviceName;

        if (hidDeviceHandle == INVALID_HANDLE_VALUE)
        {
            PRINT_WINDOWS_ERROR("CreateFile failed!");
            return L"";
        }

        wchar_t serialNumberBuffer[256] = { 0 };
        if (!HidD_GetSerialNumberString(hidDeviceHandle, serialNumberBuffer, sizeof(serialNumberBuffer)))
        {
            PRINT_WINDOWS_ERROR("HidD_GetSerialNumberString failed!");
            CloseHandle(hidDeviceHandle);
            return L"";
        }
        CloseHandle(hidDeviceHandle);
        return std::wstring(serialNumberBuffer);
    }

    bool RefreshGamepadCapabilities(HANDLE rawInputDeviceHandle, HIDra::GamepadPlatformData& windowsData)
    {
        UINT nameSize;
        GetRawInputDeviceInfo(rawInputDeviceHandle, RIDI_DEVICENAME, nullptr, &nameSize);

        char* deviceName = new char[nameSize];
        if (!GetRawInputDeviceInfo(rawInputDeviceHandle, RIDI_DEVICENAME, deviceName, &nameSize))
        {
            PRINT_WINDOWS_ERROR("Failed to get device name during capabilities refresh!");
            delete[] deviceName;
            return false;
        }

        HANDLE hidDeviceHandle = CreateFile((LPCSTR)deviceName,
                                            0, // GENERIC_READ | GENERIC_WRITE,
                                            FILE_SHARE_READ | FILE_SHARE_WRITE,
                                            nullptr,
                                            OPEN_EXISTING,
                                            0,
                                            nullptr);
        delete[] deviceName;

        PHIDP_PREPARSED_DATA preparsedData;
        if (!HidD_GetPreparsedData(hidDeviceHandle, &preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidD_GetPreparsedData failed during capabilities refresh!");
            CloseHandle(hidDeviceHandle);
            return false;
        }

        HIDP_CAPS capabilities;
        if (!HidP_GetCaps(preparsedData, &capabilities))
        {
            PRINT_WINDOWS_ERROR("HidP_GetCaps failed during capabilities refresh!");
            HidD_FreePreparsedData(preparsedData);
            CloseHandle(hidDeviceHandle);
            return false;
        }

        // Free old capabilities
        if (windowsData.m_preparsedData)
        {
            HidD_FreePreparsedData(static_cast<PHIDP_PREPARSED_DATA>(windowsData.m_preparsedData));
        }
        if (windowsData.m_buttonCapabilities)
        {
            delete[] static_cast<PHIDP_BUTTON_CAPS>(windowsData.m_buttonCapabilities);
        }
        if (windowsData.m_valueCapabilities)
        {
            delete[] static_cast<PHIDP_VALUE_CAPS>(windowsData.m_valueCapabilities);
        }

        // Allocate and populate new button capabilities
        HIDra::HIDra_UInt16 buttonCapabilitiesCount = capabilities.NumberInputButtonCaps;
        if (buttonCapabilitiesCount == 0)
        {
            PRINT_ERROR("Gamepad has 0 buttons after capabilities refresh!");
            HidD_FreePreparsedData(preparsedData);
            CloseHandle(hidDeviceHandle);
            return false;
        }

        windowsData.m_buttonCapabilities = new HIDP_BUTTON_CAPS[buttonCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS
            != HidP_GetButtonCaps(HidP_Input,
                                  static_cast<PHIDP_BUTTON_CAPS>(windowsData.m_buttonCapabilities),
                                  &buttonCapabilitiesCount,
                                  preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetButtonCaps failed during capabilities refresh!");
            HidD_FreePreparsedData(preparsedData);
            CloseHandle(hidDeviceHandle);
            return false;
        }

        // Allocate and populate new value capabilities
        windowsData.m_valueCapabilitiesCount = capabilities.NumberInputValueCaps;
        if (windowsData.m_valueCapabilitiesCount == 0)
        {
            PRINT_ERROR("Gamepad has 0 axes after capabilities refresh!");
            HidD_FreePreparsedData(preparsedData);
            CloseHandle(hidDeviceHandle);
            return false;
        }

        windowsData.m_valueCapabilities = new HIDP_VALUE_CAPS[windowsData.m_valueCapabilitiesCount];
        if (HIDP_STATUS_SUCCESS
            != HidP_GetValueCaps(HidP_Input,
                                 static_cast<PHIDP_VALUE_CAPS>(windowsData.m_valueCapabilities),
                                 &windowsData.m_valueCapabilitiesCount,
                                 preparsedData))
        {
            PRINT_WINDOWS_ERROR("HidP_GetValueCaps failed during capabilities refresh!");
            HidD_FreePreparsedData(preparsedData);
            CloseHandle(hidDeviceHandle);
            return false;
        }

        windowsData.m_reportSize = capabilities.InputReportByteLength;
        windowsData.m_preparsedData = preparsedData;
        CloseHandle(hidDeviceHandle);
        return true;
    }

    HIDra::Gamepad const* GetGamepadByHandle(HANDLE deviceHandle)
    {
        HIDra::GamepadHandler const& gamepadHandler = HIDra::Core::GetInstanceConst().GetGamepadHandler();

        HIDra::GamepadID foundGamepadID = gamepadHandler.FindGamepad([&deviceHandle](HIDra::Gamepad const& gamepad) {
            HIDra::GamepadPlatformData const& windowsData = gamepad.GetPlatformSpecificData();
            return deviceHandle == windowsData.m_lastKnownDeviceHandle;
        });

        if (foundGamepadID.IsValid())
        {
            return gamepadHandler.GetGamepad(foundGamepadID);
        }

        // Not found! Let's compare serial numbers...
        std::wstring serialNumber = GetDeviceSerialNumber(deviceHandle);
        foundGamepadID = gamepadHandler.FindGamepad([&deviceHandle, &serialNumber](HIDra::Gamepad const& gamepad) {
            HIDra::GamepadPlatformData const& windowsData = gamepad.GetPlatformSpecificData();
            return serialNumber.compare(windowsData.m_deviceSerialNumber) == 0;
        });

        if (foundGamepadID.IsValid())
        {
            // Could this be the source of the bug where gamepad input suddenly dies?
            HIDra::Gamepad const* gamepad = gamepadHandler.GetGamepad(foundGamepadID);
            // TODO: DO NOT DO THIS: THIS IS JUST FOR TESTING
            // PLEASE REFACTOR SO THAT CONST_CAST ISN'T NEEDED
            HIDra::GamepadPlatformData& windowsData =
              const_cast<HIDra::GamepadPlatformData&>(gamepad->GetPlatformSpecificData());

            if (!RefreshGamepadCapabilities(deviceHandle, windowsData))
            {
                PRINT_ERROR("Failed to refresh gamepad capabilities!");
                return nullptr;
            }

            windowsData.m_lastKnownDeviceHandle = deviceHandle;
            return gamepad;
        }

        return nullptr;
    }

    HIDra::GamepadID ProcessGamepadRawInputData(RAWINPUT* rawInputData, HIDra::GamepadInputData& outInputReport)
    {
        HIDra::Gamepad const* gamepad = GetGamepadByHandle(rawInputData->header.hDevice);
        if (!gamepad)
        {
            return HIDra::GamepadID::InvalidGamepadID;
        }

        HIDra::GamepadPlatformData const& windowsData = gamepad->GetPlatformSpecificData();

        BYTE* report = rawInputData->data.hid.bRawData;
        HIDra::HIDra_UInt32 reportSize = rawInputData->data.hid.dwCount * rawInputData->data.hid.dwSizeHid;

        // Parse Button Presses
        USAGE_AND_PAGE usages[64] = {};
        ULONG usageLength = 64;

        PHIDP_PREPARSED_DATA preparsedData = static_cast<PHIDP_PREPARSED_DATA>(windowsData.m_preparsedData);

        if (report[0] == 0x30)
        {
            DecodeRawInputReport(report, gamepad->GetVendorID(), gamepad->GetProductID(), outInputReport);
        }
        else
        {

            NTSTATUS buttonResult = HidP_GetUsagesEx(HidP_Input,
                                                     HIDP_LINK_COLLECTION_ROOT,
                                                     usages,
                                                     &usageLength,
                                                     preparsedData,
                                                     (PCHAR)report,
                                                     reportSize);

            if (buttonResult != HIDP_STATUS_SUCCESS)
            {
                switch (buttonResult)
                {
                    case HIDP_STATUS_INVALID_REPORT_LENGTH:
                        PRINT_ERROR("HidP_GetUsages Failed! The report length is not valid.");
                        break;
                    case HIDP_STATUS_INVALID_REPORT_TYPE:
                        PRINT_ERROR("HidP_GetUsages Failed! The specified report type is not valid.");
                        break;
                    case HIDP_STATUS_BUFFER_TOO_SMALL:
                        PRINT_ERROR(
                          "HidP_GetUsages Failed! The UsageList buffer is too small to hold all the usages that "
                          "are currently set to ON on the specified usage page.");
                        break;
                    case HIDP_STATUS_INCOMPATIBLE_REPORT_ID:
                        PRINT_ERROR("HidP_GetUsages Failed! The collection contains buttons on the specified usage "
                                    "page in a report "
                                    "of the specified type, but there are no such usages in the specified report.");
                        break;
                    case HIDP_STATUS_INVALID_PREPARSED_DATA:
                        PRINT_ERROR("HidP_GetUsages Failed! The preparsed data is not valid.");
                        break;
                    case HIDP_STATUS_USAGE_NOT_FOUND:
                        PRINT_ERROR(
                          "HidP_GetUsages Failed! The collection does not contain any buttons on the specified "
                          "usage page in any report of the specified report type.");
                        break;
                }
                return HIDra::GamepadID::InvalidGamepadID;
            }

            // Build Windows Gamepad Report
            HIDra::GamepadReport_Windows inputReport;
            inputReport.m_heldButtonIDs.resize(usageLength);
            // TODO: Find a better way to do this
            // inputReport.m_axes.reserve(gamepad->GetPredictedAxisCount());

            for (ULONG i = 0; i < usageLength; ++i)
            {
                inputReport.m_heldButtonIDs[i] = usages[i].Usage;
            }

            // Parse Axis Values
            const HIDra::HIDra_UInt32 valueCapabilitiesCount = windowsData.m_valueCapabilitiesCount;
            PHIDP_VALUE_CAPS valueCapabilities = static_cast<PHIDP_VALUE_CAPS>(windowsData.m_valueCapabilities);

            for (HIDra::HIDra_UInt32 i = 0; i < valueCapabilitiesCount; ++i)
            {
                ULONG value = 0;
                NTSTATUS result = HidP_GetUsageValue(HidP_Input,
                                                     valueCapabilities[i].UsagePage,
                                                     0,
                                                     valueCapabilities[i].NotRange.Usage,
                                                     &value,
                                                     preparsedData,
                                                     (PCHAR)report,
                                                     reportSize);
                if (result != HIDP_STATUS_SUCCESS)
                {
                    continue;
                }
                inputReport.m_axes.push_back(
                  HIDra::GamepadReport_Windows::Axis(valueCapabilities[i].NotRange.Usage,
                                                     static_cast<HIDra::HIDra_UInt16>(value)));
            }

            inputReport.m_vendorID = gamepad->GetVendorID();
            inputReport.m_productID = gamepad->GetProductID();

            // Translate to HIDra gamepad report
            DecodeWindowsReport(inputReport, outInputReport);
        }
        return gamepad->GetID();
    }
} // namespace
#endif // HIDra_Gamepad