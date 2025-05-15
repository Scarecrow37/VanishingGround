#include "pch.h"
#include "EditorWindowMenu.h"

EditorMenuTools::EditorMenuTools(EditorDockWindow* focusWindow) 
    :  _focusWindow(focusWindow) 
{
}

void EditorMenuTools::OnStartGui() 
{
    if (nullptr == _focusWindow)
    {
        return;
    }
    _dockWindowTable = _focusWindow->GetRefDockWindowTable();
    _editorToolTable = _focusWindow->GetRefToolTable();
}

void EditorMenuTools::OnMenu()
{
    if (ImGui::BeginMenu("Window"))
    {
        if (ImGui::BeginMenu("Tools"))
        {
            for (auto& [key, tool] : _dockWindowTable)
            {
                bool        active = tool->IsVisible();
                const char* label  = tool->GetLabel().c_str();
                if (ImGui::MenuItem(label, nullptr, active))
                {
                    tool->SetVisible(active ? false : true);
                }
            }
            for (auto& [key, tool] : _editorToolTable)
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
