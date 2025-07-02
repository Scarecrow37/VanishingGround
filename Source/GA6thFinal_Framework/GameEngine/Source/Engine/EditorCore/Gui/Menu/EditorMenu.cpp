#include "pch.h"
#include "EditorMenu.h"

EditorMenu::EditorMenu() 
{
}

EditorMenu::~EditorMenu() 
{
}

void EditorMenu::OnDrawGui()
{
    if (ImGui::BeginMenuBar())
    {
        OnMenu();

        ImGui::EndMenuBar();
    }
}
