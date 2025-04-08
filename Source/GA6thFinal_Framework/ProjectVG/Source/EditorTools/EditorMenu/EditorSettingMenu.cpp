#include "EditorSettingMenu.h"

void EditorMenuDebug::OnSelected()
{
    _isDebugMode = true == _isDebugMode ? false : true;
    Global::editorManager->SetDebugMode(_isDebugMode);
}

void EditorMenuStyleEditor::OnSelected()
{
    _isOpenStyleEditor = true == _isOpenStyleEditor ? false : true;
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
