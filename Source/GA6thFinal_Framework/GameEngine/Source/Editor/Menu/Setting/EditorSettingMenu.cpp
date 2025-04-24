#include "pch.h"
#include "EditorSettingMenu.h"

void EditorMenuDebug::OnMenu()
{
    if (ImGui::MenuItem("Debug", "", &_isDebugMode))
    {
        Global::editorModule->SetDebugMode(_isDebugMode);
    }
}

void EditorMenuStyleEditor::OnMenu()
{
    if (ImGui::MenuItem("Style", ""))
    {
        _isOpenGui = !_isOpenGui;
    }
}

void EditorMenuStyleEditor::OnTickGui()
{
    if (true == _isOpenGui)
    {
        ImGui::Begin("StyleEditor", &_isOpenGui, ImGuiWindowFlags_NoDocking);
        ImGui::ShowStyleEditor();
        ImGui::End();
    }
}

void EditorMenuFileSystemSetting::OnMenu() 
{
    if (ImGui::MenuItem("Setting", ""))
    {
        _isOpenGui = !_isOpenGui;
    }
}

void EditorMenuFileSystemSetting::OnTickGui() 
{
    if (true == _isOpenGui)
    {
        ImGui::Begin("FileSystemSetting", &_isOpenGui, ImGuiWindowFlags_NoDocking);
        UmFileSystem.DrawGuiSettingEditor();
        ImGui::End();
    }
}
