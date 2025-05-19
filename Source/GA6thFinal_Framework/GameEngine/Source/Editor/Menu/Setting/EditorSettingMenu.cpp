#include "pch.h"
#include "EditorSettingMenu.h"

void EditorMenuEditorSetting::OnMenu()
{
    EditorModule* editor = Global::editorModule;
    if (ImGui::BeginMenu("Setting"))
    {
        if (ImGui::BeginMenu("Editor"))
        {
            _isDebugMode = Global::editorModule->IsDebugMode();
            if (ImGui::MenuItem("Debug", "", &_isDebugMode))
            {
                editor->SetDebugMode(_isDebugMode);
            }
            if (ImGui::MenuItem("Style"))
            {
                _isOpenGui = !_isOpenGui;
            }
            if (ImGui::MenuItem("Reset Layout"))
            {
                editor->ResetGuiLayout();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void EditorMenuEditorSetting::OnTickGui() 
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
    if (ImGui::BeginMenu("Setting"))
    {
        if (ImGui::BeginMenu("FileSystem"))
        {
            if (ImGui::MenuItem("Setting", ""))
            {
                _isOpenGui = !_isOpenGui;
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void EditorMenuFileSystemSetting::OnTickGui() 
{
    if (true == _isOpenGui)
    {
        ImGuiWindowFlags flags = ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_MenuBar;
        ImGui::Begin("FileSystemSetting", &_isOpenGui, flags);
        UmFileSystem.DrawGuiSettingEditor();
        ImGui::End();
    }
}
