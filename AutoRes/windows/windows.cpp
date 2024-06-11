#include "windows.h"
#include <iostream>
#include <Windows.h>

bool windows::change_resolution(const int& width, const int& height, const int& bits_per_pixel) // i havnt played with modifying the display so thanks chatgpt 
{
    DEVMODE dm;
    ZeroMemory(&dm, sizeof(dm));
    dm.dmSize = sizeof(dm);
    dm.dmPelsWidth = width;
    dm.dmPelsHeight = height;
    dm.dmBitsPerPel = bits_per_pixel;
    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    LONG result = ChangeDisplaySettings(&dm, CDS_TEST);
    if (result != DISP_CHANGE_SUCCESSFUL)
    {
        // Test failed, return false
        return false;
    }

    result = ChangeDisplaySettings(&dm, 0);
    if (result != DISP_CHANGE_SUCCESSFUL)
    {
        // Change failed, return false
        return false;
    }

    return true;
}

void windows::set_startup_program(const std::string& exe_name, const std::string& exe_path, const bool& enable)
{
    // Define the registry path for startup programs
    HKEY hKey;
    const char* regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";

    // Open the registry key
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_SET_VALUE, &hKey);
    if (result != ERROR_SUCCESS) {
        std::cerr << "Error opening registry key: " << result << std::endl;
        return;
    }

    if (enable) {
        // Set the value in the registry key to enable startup
        result = RegSetValueExA(
            hKey,                    // Handle to the open key
            exe_name.c_str(),         // Name of the value to set
            0,                       // Reserved, must be zero
            REG_SZ,                  // Type of data (string)
            (const BYTE*)exe_path.c_str(), // Data to set
            (DWORD)(exe_path.size() + 1)  // Size of the data
        );

        if (result != ERROR_SUCCESS) {
            std::cerr << "Error setting registry value: " << result << std::endl;
        } else {
            std::cout << "Successfully set startup program." << std::endl;
        }
    } else {
        // Delete the value from the registry key to disable startup
        result = RegDeleteValueA(hKey, exe_name.c_str());

        if (result != ERROR_SUCCESS) {
            std::cerr << "Error deleting registry value: " << result << std::endl;
        } else {
            std::cout << "Successfully removed startup program." << std::endl;
        }
    }

    // Close the registry key
    RegCloseKey(hKey);
}
