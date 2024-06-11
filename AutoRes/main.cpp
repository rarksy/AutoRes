#include "backend/backend.h"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include <glfw/glfw3.h>

#include "menu/menu.h"

int main(int argc, char* argv[])
{
    if (!glfwInit())
        throw std::exception("failed to init glfw");
    const auto window = glfwCreateWindow(550, 400, "AutoRes++", nullptr, nullptr);
    glfwMakeContextCurrent(window);

    backend be;
    be.load_instances();
    be.application_detection();

    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ImGui::GetIO().IniFilename = nullptr;

    while (!glfwWindowShouldClose(window))
    {
        glfwGetWindowSize(window, &be.window_size_x, &be.window_size_y);
        glfwPollEvents();

        menu::render(be);

        glfwSwapBuffers(window);
    }

    be.active = false;

    glfwTerminate();
    
    return 0;
}
