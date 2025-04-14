#include "EditorProjectMenu.h"

void EditorMenuScriptBuilder::OnMenu()
{
    std::string label = GetLabel();
    if (ImGui::MenuItem(label.c_str(), ""))
    {
        Global::engineCore->ComponentFactory.InitalizeComponentFactory();
    }
}
