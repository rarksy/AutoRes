#pragma once
#include <imgui/imgui.h>
#include "../backend/backend.h"

namespace menu
{
    constexpr auto flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;
    
    void render(backend& be);
    void show(backend& be);
}
