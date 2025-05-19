#include "pch.h"
#include "EditorModelMenu.h"

using namespace Global;
using namespace u8_literals;

EditorModelMenu::EditorModelMenu()
{
    _save = EditorIcon::ICON_FILE_SAVE;
    _save += " Save";

    _saveAs = EditorIcon::ICON_FILE_SAVE;
    _saveAs += " Save As";

    _imoprt = EditorIcon::ICON_FOLDER_OPEN;
    _imoprt += " Import FBX";

    _open = EditorIcon::ICON_FOLDER_OPEN;
    _open += " Open Model";
}

void EditorModelMenu::OnStartGui()
{
    auto&             system    = Global::editorModule->GetDockWindowSystem();
    EditorDockWindow* modelDock = system.GetDockWindow("ModelDock");
    _editorModelDetails         = modelDock->GetGui<EditorModelDetails>();
}

void EditorModelMenu::OnMenu()
{
    EditorModule& editor = *Global::editorModule;

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem(_imoprt.c_str()))
        {
            _editorModelDetails->ImportModel();
        }

        if (ImGui::MenuItem(_open.c_str()))
        {
            _editorModelDetails->ImportModel();
        }

        if (ImGui::MenuItem(_save.c_str()))
        {
            _editorModelDetails->SaveModel();
        }

        if (ImGui::MenuItem(_saveAs.c_str()))
        {
            _editorModelDetails->ExportModel();
        }

        ImGui::EndMenu();
    }   
}