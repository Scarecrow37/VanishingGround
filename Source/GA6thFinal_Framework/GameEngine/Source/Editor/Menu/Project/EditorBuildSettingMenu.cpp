#include "pch.h"
using namespace u8_literals;

EditorBuildSettingMenu::EditorBuildSettingMenu() 
    : 
    isPopup(false), 
    isShow(false)
{
}

void EditorBuildSettingMenu::OnTickGui()
{
    if (isPopup)
    {
        ImGui::PushID(this);
        if (isShow == false)
        {
            ImVec2 viewportCenter = ImGui::GetMainViewport()->GetCenter();
            ImGui::SetNextWindowPos(viewportCenter, ImGuiCond_Always, ImVec2(0.5f, 0.5f));
            ImGui::SetNextWindowSize(ImVec2(475, 425), ImGuiCond_FirstUseEver);
            isShow = true;
        }
        ImGui::Begin("Build Setting", &isPopup, ImGuiWindowFlags_NoDocking);
        {
            BuildSettingPopup();
        }
        ImGui::End();
        ImGui::PopID();
    }
}

void EditorBuildSettingMenu::OnMenu()
{
    EditorModule& editorModule = *Global::editorModule;
    EComponentFactory& componentFactory = UmComponentFactory;

    if (ImGui::BeginMenu("Project"))
    {
        if (ImGui::BeginMenu("Build"))
        {
            if (ImGui::MenuItem("Build Project"))
            {
                isPopup = true;
                isShow  = false; 
            }
            if (ImGui::MenuItem("Build Script"))
            {
                componentFactory.InitalizeComponentFactory();
            }
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }
}

void EditorBuildSettingMenu::BuildSettingPopup() 
{
    EditorModule& editorModule = *Global::editorModule;

    if(ImGui::BeginChild("Start Scene Setting", {0, 300}, ImGuiChildFlags_Border))
    {
        std::string& startSceneSetting = ESceneManager::Engine::GetStartSceneSetting();
        ImGui::Text("Start Scene Setting");
        ImGuiHelper::HoveredToolTip(u8"게임이 처음으로 로드할 장면을 설정합니다."_c_str);
        ImGui::BeginDisabled();
        ImGui::Button(startSceneSetting.c_str());
        ImGui::EndDisabled();
        ImGui::Separator();
        for (auto& [guid, scene] : UmSceneManager.GetScenesMap())
        {
            File::Path scenePath = guid.ToPath();
            std::string path = std::filesystem::relative(scenePath, UmFileSystem.GetRootPath()).string();
            if (ImGui::Button(path.c_str()))
            {
                startSceneSetting = path;
                UmSceneManager.SaveSettingFile();
            }
            const std::string toolTip = std::format("{}{}", path, u8"으로 설정합니다."_c_str);
            ImGuiHelper::HoveredToolTip(toolTip);
        }
        ImGui::EndChild();

        static std::string buildOutPath;
        ImGui::InputText("Save Path", &buildOutPath, ImGuiInputTextFlags_ReadOnly);
        if (ImGui::BeginItemTooltip())
        {
            ImGui::Text(buildOutPath.c_str());
            ImGui::EndTooltip();
        }
        ImGui::SameLine();
        if (ImGui::Button(EditorIcon::ICON_FOLDER_OPEN))
        {
            File::Path path;
            if (File::ShowOpenFolderBrowser(UmApplication.GetHwnd(), L"저장할 폴더를 선택하세요.", L"C:", path))
            {
                buildOutPath = path.string();
            }  
        }
        if (ImGui::Button("Build"))
        {
            if (false == buildOutPath.empty())
            {
                bool result = editorModule.BuildSystem.BuildProject(buildOutPath.c_str());
                if (true == result)
                {
                    MessageBox(UmApplication.GetHwnd(), L"빌드 완료.", L"빌드", MB_OK);
                }
            }
        }
        ImGui::SameLine();
        if (ImGui::Button("Close"))
        {
            isPopup = false;
        }
    }
}
