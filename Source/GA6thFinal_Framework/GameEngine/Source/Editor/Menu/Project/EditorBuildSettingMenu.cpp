#include "pch.h"
using namespace u8_literals;

EditorBuildSettingMenu::EditorBuildSettingMenu() 
    : 
    isPopup(false), 
    isShow(false)
{
    SetCallOrder(1);
    SetPath("Project/Build");
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
            ImGui::SetNextWindowSize(ImVec2(475, 375), ImGuiCond_FirstUseEver);
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
    if (ImGui::MenuItem("Build Setting"))
    {
        isPopup = true;
        isShow  = false;
    }
}

void EditorBuildSettingMenu::BuildSettingPopup() 
{
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
            std::string path = guid.ToPath().string();
            if (ImGui::Button(path.c_str()))
            {
                startSceneSetting = path;
            }
            std::string toolTip = std::format("{}{}", path, u8"으로 설정합니다."_c_str);
            ImGuiHelper::HoveredToolTip(toolTip);
        }
        ImGui::EndChild();
    }
}
