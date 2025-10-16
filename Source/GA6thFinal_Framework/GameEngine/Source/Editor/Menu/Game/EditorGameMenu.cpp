#include "pch.h"
#include "EditorGameMenu.h"

EditorGameMenu::EditorGameMenu() 
{
}

void EditorGameMenu::OnStartGui()
{
    auto&             system = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* dock   = system.GetDockWindow("Scene##dock");
    if (dock)
    {
        _gameView = dock->GetGui<EditorGameView>();
    }
}

void EditorGameMenu::OnMenu() 
{
    if (_gameView)
    {
        if (ImGui::BeginMenu("Game"))
        {
            ImGuiHelper::AlignedText("Camera Mode", ImGuiHelper::CENTER, 0.8f);
            ImGui::Separator();
            _gameView->ShowPopupCameraMode();
            ImGui::EndMenu();
        }
    }
}
