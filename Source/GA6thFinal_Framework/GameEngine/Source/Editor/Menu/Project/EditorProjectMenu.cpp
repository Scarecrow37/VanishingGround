#include "pch.h"
#include "EditorProjectMenu.h"

void EditorMenuScriptBuilder::OnMenu()
{
    if (ImGui::MenuItem("Script Build", ""))
    {
        Global::engineCore->ComponentFactory.InitalizeComponentFactory();
    }
}

static void ShowNewProjectPopup()
{
    static File::Path directory = ".";
    static File::Path extension = ".UmProject";
    static char pathBuffer[128] = "";
    static char nameBuffer[128] = "New Project";
    static char defaultName[]   = "New Project";
    File::Path path = directory / nameBuffer;
    path.replace_extension(extension);
    path = File::GenerateUniquePath(path);

    std::string pathStr = path.generic_string();
    strcpy_s(pathBuffer, pathStr.c_str());
    ImGui::Text("Project Directory: ");
    //ImGui::BeginDisabled();
    ImGui::InputText("##path_input", pathBuffer, IM_ARRAYSIZE(pathBuffer), ImGuiInputTextFlags_ReadOnly);
    //ImGui::EndDisabled();
    if (ImGui::BeginItemTooltip())
    {
        ImGui::Text(pathStr.c_str());
        ImGui::EndTooltip();
    }
    ImGui::SameLine();
    if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN))
    {
        TCHAR title[] = L"폴더를 선택하세요.";
        UINT  flags   = BIF_USENEWUI | BIF_RETURNONLYFSDIRS;
        
        if (File::OpenForderBrowser(title, flags, directory))
        {
        }
    }

    
    ImGui::Text("Project Name: ");
    ImGui::InputText("##name_input", nameBuffer, IM_ARRAYSIZE(nameBuffer), ImGuiInputTextFlags_None);

    ImGuiHelper::Separator();

    if (ImGui::Button("Create##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Enter))
    {
        UmFileSystem.CreateProject(path);
        UmFileSystem.LoadProject(path);
        strcpy_s(nameBuffer, defaultName);
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();

    if (ImGui::Button("Cancel##") || ImGui::IsKeyReleased(ImGuiKey::ImGuiKey_Escape))
    {
        ImGui::CloseCurrentPopup();
    }
}

void EditorMenuProjectRoot::OnMenu() 
{
    if (ImGui::MenuItem("New Project", nullptr))
    {
        Global::editorModule->OpenPopupBox("New Project", ShowNewProjectPopup);
    }
    if (ImGui::MenuItem("Open Project", nullptr))
    {
        TCHAR filter[] = L"프로젝트 파일 (.UmProject)\0*.UmProject\0모든 파일\0*.*\0";

        File::Path out;
        if (File::OpenFileNameBrowser(filter, out))
        {
            UmFileSystem.LoadProject(out);
        }
    }
    if (ImGui::MenuItem("Save Project", nullptr))
    {
        Global::engineCore->ComponentFactory.InitalizeComponentFactory();
    }
}

