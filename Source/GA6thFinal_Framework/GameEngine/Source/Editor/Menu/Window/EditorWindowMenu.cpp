#include "pch.h"
#include "EditorWindowMenu.h"

EditorMenuTools::EditorMenuTools(EditorDockWindow* focusWindow) 
    :  _focusWindow(focusWindow) 
{
}

void EditorMenuTools::OnMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::BeginMenu("Tools"))
        {
            auto& table = _focusWindow->GetRefToolTable();
            for (auto& [key, tool] : table)
            {
                bool active = tool->IsVisible();
                const char* label  = tool->GetLabel().c_str();
                if (ImGui::MenuItem(label, nullptr, active))
                {
                    tool->SetVisible(active ? false : true);
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}
