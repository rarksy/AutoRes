#pragma once
#include <string>

namespace windows
{
    bool change_resolution(const int& width, const int& height, const int& bits_per_pixel = 32);
    void set_startup_program(const std::string& exe_name, const std::string& exe_path, const bool& enable);
};
