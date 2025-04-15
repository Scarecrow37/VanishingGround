#include "EditorSettingMenu.h"

void EditorMenuDebug::OnMenu()
{
    if (ImGui::MenuItem("Debug", "", &_isDebugMode))
    {
        Global::editorManager->SetDebugMode(_isDebugMode);
    }
}

void EditorMenuStyleEditor::OnMenu()
{
    ImGui::MenuItem("Style", "", &_isOpenStyleEditor);
}

void EditorMenuStyleEditor::OnTickGui()
{
    if (true == _isOpenStyleEditor)
    {
        ImGui::Begin("StyleEditor", &_isOpenStyleEditor, ImGuiWindowFlags_NoDocking);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
}
