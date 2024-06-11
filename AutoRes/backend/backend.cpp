#include "backend.h"
#include <Windows.h>
#include "misc_lib.hpp"

backend::backend()
{
    this->default_resolution = vec2(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    this->exe_directory = ml::get_exe_directory().parent_path().string();

    if (!ml::file_or_directory_exists(this->exe_directory.string() + "\\data\\"))
        ml::create_directory(this->exe_directory.string() + "\\data\\");

    this->active = true;
}

backend::~backend()
{
    this->active = false;
}

void backend::application_detection()
{
    std::thread det_thread([this]
    {
        while (this->active)
        {
            for (auto& inst : this->all_instances)
            {
                const bool exe_running = ml::is_exe_running(ml::string_t_wstring(inst.exe_name));

                if (exe_running && !inst.running)
                {
                    change_resolution(inst.target_resolution.x, inst.target_resolution.y);
                    inst.running = true;
                }
                else if (!exe_running && inst.running)
                {
                    change_resolution(this->default_resolution.x, this->default_resolution.y);
                    inst.running = false;
                }
            }

            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    });
    det_thread.detach();
}

void backend::load_instances() {
    std::ifstream file(this->exe_directory.string() + this->instances_location);

    if (!file.is_open())
        return;

    nlohmann::json j = nlohmann::json::parse(file);
    all_instances.clear();
    for (const auto& item : j)
    {
        instance inst;
        inst.label = item["label"];
        const std::string exe_path = item["exe_path"];
        inst.exe_path = std::filesystem::path(exe_path);
        inst.exe_name = item["exe_name"];
        inst.target_resolution.x = item["res_x"];
        inst.target_resolution.y = item["res_y"];
        all_instances.push_back(inst);
    }
}

void backend::add_new_instance(const std::string& label, const std::string& exe_path, const std::string& exe_name, const int& res_x, const int& res_y) {
    instance new_instance;
    new_instance.label = label;
    new_instance.exe_path = exe_path;
    new_instance.exe_name = exe_name;
    new_instance.target_resolution = {res_x, res_y};
    new_instance.exe_icon = 0;

    all_instances.push_back(new_instance);

    // Update JSON data with integer index
    nlohmann::json data;
    for (size_t i = 0; i < all_instances.size(); ++i) {
        data[i]["label"] = all_instances[i].label;
        data[i]["exe_path"] = all_instances[i].exe_path;
        data[i]["exe_name"] = all_instances[i].exe_name;
        data[i]["res_x"] = all_instances[i].target_resolution.x;
        data[i]["res_y"] = all_instances[i].target_resolution.y;
    }

    ml::json_write_data(this->exe_directory.string() + this->instances_location, data);
}

void backend::update_instance(const int& index) const {
    nlohmann::json data = ml::json_get_data_from_file(this->exe_directory.string() + this->instances_location);

    if (index >= 0 && index < all_instances.size()) {
        const auto& instance = all_instances[index];
        data[index]["label"] = instance.label;
        data[index]["exe_path"] = instance.exe_path;
        data[index]["exe_name"] = std::filesystem::path(instance.exe_path).filename();
        data[index]["res_x"] = instance.target_resolution.x;
        data[index]["res_y"] = instance.target_resolution.y;
    }

    ml::json_write_data(this->exe_directory.string() + this->instances_location, data);
}

void backend::remove_instance(const int& index) {
    nlohmann::json data = ml::json_get_data_from_file(this->exe_directory.string() + this->instances_location);

    if (index >= 0 && index < all_instances.size()) {
        data.erase(index);
        this->all_instances.erase(this->all_instances.begin() + index);

        // Update JSON data after removal
        for (size_t i = 0; i < all_instances.size(); ++i) {
            data[i]["label"] = all_instances[i].label;
            data[i]["exe_path"] = all_instances[i].exe_path;
            data[i]["exe_name"] = all_instances[i].exe_name;
            data[i]["res_x"] = all_instances[i].target_resolution.x;
            data[i]["res_y"] = all_instances[i].target_resolution.y;
        }

        ml::json_write_data(this->exe_directory.string() + this->instances_location, data);
    }
}

void backend::set_startup_program(const std::string& exe_name, const std::string& exe_path, const bool& enable) { // i havnt played with registry before so thanks chatgpt
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

bool backend::change_resolution(const int& width, const int& height, const int& bits_per_pixel) // i havnt played with modifying the display so thanks chatgpt 
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
