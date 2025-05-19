#include "pch.h"
#include "EditorProjectMenu.h"

void EditorMenuScriptBuilder::OnMenu()
{
    if (ImGui::BeginMenu("Project"))
    {
        if (ImGui::BeginMenu("Build"))
        {
            if (ImGui::MenuItem("Script Build", ""))
            {
                Global::engineCore->ComponentFactory.InitalizeComponentFactory();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
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
        HWND    owner   = UmApplication.GetHwnd();
        LPCWSTR title   = L"새 프로젝트 만들기";
        if (File::ShowOpenFolderBrowser(owner, title, L"", directory))
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
    if (ImGui::BeginMenu("Project"))
    {
        if (ImGui::MenuItem("New Project", nullptr))
        {
            Global::editorModule->OpenPopupBox("New Project", ShowNewProjectPopup);
        }
        if (ImGui::MenuItem("Open Project", nullptr))
        {
            HWND       owner    = UmApplication.GetHwnd();
            LPCWSTR    title    = L"새 프로젝트 만들기";
            std::vector<File::Path> out;
            if (File::ShowOpenFileBrowser(owner, title, L"",
                {
                    {L"프로젝트 파일\0", L"*.UmProject*\0"},
                    {L"모든 파일\0", L"*.*\0"}
                },
                false, out))
            {
                UmFileSystem.LoadProject(out.front());
            }
        }
        if (ImGui::MenuItem("Save Project", nullptr))
        {
            UmFileSystem.SaveProjectWithMessageBox();
        }
        if (ImGui::MenuItem("SaveAs Project", nullptr))
        {
            HWND       owner    = UmApplication.GetHwnd();
            LPCWSTR    title    = L"다른 이름으로 저장";
            File::Path curPath  = UmFileSystem.GetRootPath();
            File::Path directory;
            if (File::ShowOpenFolderBrowser(owner, title, curPath.c_str(), directory))
            {
                if (true == UmFileSystem.SaveAsProject(directory))
                {
                    File::Path projectName = UmFileSystem.GetProjectName();
                    projectName.replace_extension(File::PROJECT_EXTENSION);
                    UmFileSystem.LoadProject(directory / projectName);
                }
            }
        }
        ImGui::EndMenu();
    }
}

