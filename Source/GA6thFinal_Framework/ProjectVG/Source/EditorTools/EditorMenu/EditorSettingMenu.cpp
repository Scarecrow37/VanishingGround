#include "EditorSettingMenu.h"

void EditorMenuDebug::OnMenu()
{
    std::string label = GetLabel();
    if (ImGui::MenuItem(label.c_str(), "", &_isDebugMode))
    {
        _isDebugMode = (true == _isDebugMode) ? false : true;
        Global::editorManager->SetDebugMode(_isDebugMode);
    }
}

void EditorMenuStyleEditor::OnMenu()
{
    std::string label = GetLabel();
    if (ImGui::MenuItem(label.c_str(), "", &_isOpenStyleEditor))
    {
        Global::editorManager->SetDebugMode(_isOpenStyleEditor);
    }
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
