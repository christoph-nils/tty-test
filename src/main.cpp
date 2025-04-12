#include <windows.h>
#include <setupapi.h>
#include <devguid.h>
#include <regstr.h>
#include <stdio.h>
#include <iostream>
#include <iterator>
#include <regex>
#include <string>

bool GetDeviceProperty(HDEVINFO hDevInfo, SP_DEVINFO_DATA devInfoData, DWORD property, LPTSTR buffer, size_t bufferSize)
{
    return SetupDiGetDeviceRegistryProperty(hDevInfo, &devInfoData, property, NULL, (PBYTE)buffer, bufferSize, NULL);
}

std::string regex_get_first_match(std::string input, std::string pattern)
{
    std::regex word_regex(pattern);
    auto match_begin = std::sregex_iterator(input.begin(), input.end(), word_regex);
    auto match_end = std::sregex_iterator();
    if (match_begin != match_end) {
        std::smatch match = *match_begin;
        std::string match_str = match.str(1);
        return match_str;
    }
    return "";
}

std::string extract_com_port(std::string friendlyName)
{
    return regex_get_first_match(friendlyName, "\\(([a-zA-Z0-9]*)\\)$");
}
std::string extract_vid(std::string hardwareID)
{
    return regex_get_first_match(hardwareID, "VID_([a-zA-Z0-9]*)");
}
std::string extract_pid(std::string hardwareID)
{
    return regex_get_first_match(hardwareID, "PID_([a-zA-Z0-9]*)");
}

void ListComPorts() {
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData;
    DWORD i;

    GUID ClassGuidList[5] = {0};
    DWORD RequiredSize = NULL;

    SetupDiClassGuidsFromName("Ports", ClassGuidList, sizeof(ClassGuidList), &RequiredSize);

    // Get the device information set for all installed devices.
    hDevInfo = SetupDiGetClassDevs(ClassGuidList, NULL, 0, DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) {
        printf("Error getting device information set\n");
        return;
    }

    // Enumerate through all devices in the Set.
    DeviceInfoData.cbSize = sizeof(SP_DEVINFO_DATA);
    for (i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++) {
        TCHAR friendlyName[256];
        TCHAR manufacturer[256];
        TCHAR hardwareID[256];

        GetDeviceProperty(hDevInfo, DeviceInfoData, SPDRP_FRIENDLYNAME, friendlyName, sizeof(friendlyName));
        GetDeviceProperty(hDevInfo, DeviceInfoData, SPDRP_MFG, manufacturer, sizeof(manufacturer));
        GetDeviceProperty(hDevInfo, DeviceInfoData, SPDRP_HARDWAREID, hardwareID, sizeof(hardwareID));

        printf("Friendly Name:    %s\n", friendlyName);
        printf("Manufacturer:     %s\n", manufacturer);
        printf("Hardware ID:      %s\n", hardwareID);
        TCHAR deviceInstanceID[256];
        // Get the device instance ID
        if (SetupDiGetDeviceInstanceId(hDevInfo, &DeviceInfoData, deviceInstanceID, sizeof(deviceInstanceID), NULL)) {
            printf("deviceInstanceID: %s\n", deviceInstanceID);
        }
        std::string com_port = extract_com_port(std::string(friendlyName));
        if (com_port.size() > 0)
            std::cout << "ComPort: " << com_port << '\n';
        std::string vid = extract_vid(std::string(hardwareID));
        if (vid.size() > 0)
            std::cout << "vid: " << vid << '\n';
        std::string pid = extract_pid(std::string(hardwareID));
        if (pid.size() > 0)
            std::cout << "pid: " << pid << '\n';
        printf("\n");
    }

    if (GetLastError() != NO_ERROR && GetLastError() != ERROR_NO_MORE_ITEMS) {
        printf("Error enumerating device info\n");
    }

    // Cleanup
    SetupDiDestroyDeviceInfoList(hDevInfo);
}

int main() {
    ListComPorts();
    return 0;
}
