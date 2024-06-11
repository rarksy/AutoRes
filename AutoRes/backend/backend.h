#pragma once
#include <filesystem>
#include <vector>
#include <string>

#include "vec2.h"

class backend
{
private:
    struct instance
    {
        bool running = false;
        
        std::string label;
        std::filesystem::path exe_path;
        std::string exe_name;

        vec2 target_resolution;

        unsigned exe_icon;
    };

    std::filesystem::path exe_directory;
    std::string instances_location = "\\data\\instances.json";

public:
    bool active = false;
    vec2 default_resolution;
    
    std::vector<instance> all_instances;
    int window_size_x;
    int window_size_y;
    
    backend();

    void application_detection();

    void load_instances();
    void add_new_instance(const std::string& label, const std::string& exe_path, const std::string& exe_name, const int& res_x, const int& res_y);
    void update_instance(const int& index);
    void remove_instance(const int& index);

    void set_startup_program(const std::string& exe_name, const std::string& exe_path, const bool& enable);
    bool change_resolution(const int& width, const int& height, const int& bits_per_pixel = 32);
};