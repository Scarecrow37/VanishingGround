#include "pch.h"
#include "EditorWindowMenu.h"

void EditorMenuTools::OnMenu()
{
    if (ImGui::BeginMenu("Tools"))
    {
        //EditorDockSpace* dockSpace = Global::editorModule->GetMainDockSpace();
        //const auto& toolTable = dockSpace->GetRefToolTable();
        //for (auto& [key, tool] : toolTable)
        //{
        //    bool active = tool->IsVisible();
        //    if (ImGui::MenuItem(tool->GetLabel().c_str(), nullptr, active,
        //                        GetActive()))
        //    {
        //        tool->SetVisible(active ? false : true);
        //    }
        //}
        //ImGui::EndMenu();
    }
}
