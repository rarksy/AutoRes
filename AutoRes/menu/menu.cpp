#include "menu.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"
#include <glfw/glfw3.h>
#include <Windows.h>

#include "misc_lib.hpp"

bool IsStartupProgramSet(const std::string& appName) {
    HKEY hKey;
    const char* regPath = "SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run";
    LONG result = RegOpenKeyExA(HKEY_CURRENT_USER, regPath, 0, KEY_QUERY_VALUE, &hKey);

    if (result != ERROR_SUCCESS) {
        return false;
    }

    DWORD type = 0;
    result = RegQueryValueExA(hKey, appName.c_str(), nullptr, &type, nullptr, nullptr);
    RegCloseKey(hKey);

    return (result == ERROR_SUCCESS && type == REG_SZ);
}

void menu::render(backend& be)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    menu::show(be);

    ImGui::Render();
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void menu::show(backend& be)
{
    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize({static_cast<float>(be.window_size_x), static_cast<float>(be.window_size_y)} );
    ImGui::Begin("AutoRes++", nullptr, flags);

    if (ImGui::Button("Add New Instance"))
        ImGui::OpenPopup("Add New Instance");

    ImGui::SameLine();

    static bool open_on_startup = IsStartupProgramSet("AutoRes");
    if (ImGui::Checkbox("Open On Startup", &open_on_startup))
        be.set_startup_program("AutoRes", ml::get_exe_directory().string(), open_on_startup);

    if (ImGui::BeginPopupModal("Add New Instance", NULL, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        static std::string label;
        ImGui::InputText("Label", &label);
        
        static std::string exe_path;
        ImGui::InputText("Executable Path", &exe_path);
        
        ImGui::SameLine();

        if (ImGui::Button("..."))
            exe_path = ml::open_file_dialog();
        
        ImGui::BeginDisabled();
        std::string exe_name = std::filesystem::path(exe_path).filename().string();
        ImGui::InputText("Executable Name", &exe_name);
        ImGui::EndDisabled();

        static int res_x = be.default_resolution.x;
        ImGui::InputInt("Target X Resolution", &res_x);

        static int res_y = be.default_resolution.y;
        ImGui::InputInt("Target Y Resolution", &res_y);
        

        if (ImGui::Button("Okay"))
        {
            be.add_new_instance(label, exe_path, exe_name, res_x, res_y);

            label.clear();
            exe_path.clear();
            exe_name.clear();
            res_x = be.default_resolution.x;
            res_y = be.default_resolution.y;
            
            ImGui::CloseCurrentPopup();
        }
        
        ImGui::EndPopup();
    }
    
    ImGui::BeginChild("Instances");

    auto& all_instances = be.all_instances;
    
    const size_t instances_size = all_instances.size();
    for (int i = 0; i < instances_size; i++)
    {
        auto& instance = all_instances[i];
        
        const std::string instance_child_label = "instance" + std::to_string(i);
        ImGui::BeginChild(instance_child_label.c_str(), {0, 50}, true);

        ImGui::BeginDisabled();
        ImGui::SetNextItemWidth(100.F);
        const std::string instance_label = "##label" + std::to_string(i);
        ImGui::InputTextWithHint(instance_label.c_str(), "Label", &instance.label);
        ImGui::EndDisabled();
        
        ImGui::SameLine();

        ImGui::SetNextItemWidth(200.F);
        const std::string instance_exe_path = "##exe" + std::to_string(i);
        if (ImGui::InputTextWithHint(instance_exe_path.c_str(), "Executable Directory", &instance.exe_path))
            be.update_instance(i);

        ImGui::SameLine();

        if (ImGui::Button("..."))
        {
            be.all_instances[i].exe_path = ml::open_file_dialog();
            be.update_instance(i);
        }

        ImGui::SameLine();

        ImGui::SetNextItemWidth(40.F);
        const std::string instance_resx_path = "##resx" + std::to_string(i);
        if (ImGui::InputInt(instance_resx_path.c_str(), &all_instances[i].target_resolution.x, NULL))
            be.update_instance(i);

        ImGui::SameLine();

        ImGui::Text("x");

        ImGui::SameLine();

        ImGui::SetNextItemWidth(40.F);
        const std::string instance_resy_path = "##resy" + std::to_string(i);
        if (ImGui::InputInt(instance_resy_path.c_str(), &all_instances[i].target_resolution.y, NULL))
            be.update_instance(i);

        ImGui::SameLine();

        if (ImGui::Button("X"))
            be.remove_instance(i);
        
        ImGui::EndChild();
    }
    
    ImGui::EndChild();
    ImGui::End();
}
