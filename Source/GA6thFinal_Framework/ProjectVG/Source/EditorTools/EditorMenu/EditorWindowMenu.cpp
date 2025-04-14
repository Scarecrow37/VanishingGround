#include "EditorWindowMenu.h"

void EditorMenuTools::OnMenu()
{
    if (ImGui::BeginMenu("Tools"))
    {
        EditorDockSpace* dockSpace = Global::editorManager->GetMainDockSpace();
        const std::map<std::string, EditorTool*>& toolTable =
            dockSpace->GetRefToolTable();
        for (auto& [key, tool] : toolTable)
        {
            bool active = tool->GetVisible();
            if (ImGui::MenuItem(tool->GetLabel().c_str(), "-", active,
                                GetActive()))
            {
                tool->SetVisible(active ? false : true);
            }
        }
        ImGui::EndMenu();
    }
}
