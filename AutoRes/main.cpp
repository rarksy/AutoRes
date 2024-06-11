#include "backend/backend.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glfw/glfw3.h>
#include <Windows.h>
#include "Fonts.hpp"
#include "menu/menu.h"

int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int)
{
    if (!glfwInit())
        throw std::exception("failed to init glfw");
    
    const auto window = glfwCreateWindow(550, 400, "AutoRes", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    backend be;
    be.load_instances();
    be.application_detection();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    auto& io = ImGui::GetIO();
    
    io.IniFilename = nullptr;
    io.Fonts->AddFontFromMemoryCompressedTTF(nunito_data, nunito_size, 20.F);

    while (!glfwWindowShouldClose(window))
    {
        const double frame_start = glfwGetTime();
        
        glfwGetWindowSize(window, &be.window_size_x, &be.window_size_y);
        glfwPollEvents();

        menu::render(be);

        glfwSwapBuffers(window);

        const double frame_end = glfwGetTime();
        const double frame_elapsed = frame_end - frame_start;
        constexpr double target_frametime = 1.0 / static_cast<double>(80);
        if (frame_elapsed < target_frametime)
        {
            const double time_to_sleep = target_frametime - frame_elapsed;
            std::this_thread::sleep_for(std::chrono::duration<double>(time_to_sleep));
        }
    }
    
    glfwTerminate();
    ImGui_ImplGlfw_Shutdown();
    ImGui_ImplOpenGL3_Shutdown();
    ImGui::DestroyContext();
    
    return 0;
}
