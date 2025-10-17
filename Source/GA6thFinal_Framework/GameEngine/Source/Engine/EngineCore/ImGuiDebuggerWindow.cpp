#include "pch.h"
#include "ImGuiDebuggerWindow.h"

void ImGuiDebuggerWindow::Update()
{
    if (ImGui::IsKeyPressed(ImGuiKey_F3, false))
    {
        DebugMode = !DebugMode;
    }
}