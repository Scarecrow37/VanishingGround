#include "EditorProjectMenu.h"

void EditorMenuScriptBuilder::OnMenu()
{
    if (ImGui::MenuItem("Script Build", ""))
    {
        Global::engineCore->ComponentFactory.InitalizeComponentFactory();
    }
}
