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
    };

    std::filesystem::path exe_directory;
    std::string instances_location = "\\data\\instances.json";

public:
    bool active = false;
    vec2 default_resolution;
    
    std::vector<instance> all_instances;
    int window_size_x = 0;
    int window_size_y = 0;
    
    backend();
    ~backend();

    void application_detection();

    void load_instances();
    void add_new_instance(const std::string& label, const std::string& exe_path, const std::string& exe_name, const int& res_x, const int& res_y);
    void update_instance(const int& index) const;
    void remove_instance(const int& index);
};