#include <engine/pch.h>

#include <engine/ui_extensions.h>

namespace cgt::ui
{

void ImGuiBeginInvisibleFullscreenWindow()
{
    auto& io = ImGui::GetIO();
    ImGui::PushStyleColor(ImGuiCol_WindowBg, IM_COL32_BLACK_TRANS);
    ImGui::SetNextWindowPos({ 0.0f, 0.0f });
    ImGui::SetNextWindowSize(io.DisplaySize);
    ImGui::Begin(
        "Invisible",
        nullptr,
        0 | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoBringToFrontOnFocus);
}

void ImGuiEndInvisibleFullscreenWindow()
{
    ImGui::End();
    ImGui::PopStyleColor(1);
}

}