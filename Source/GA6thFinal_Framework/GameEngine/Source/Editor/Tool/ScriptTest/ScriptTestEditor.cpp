#include "pch.h"
#include "ScriptTestEditor.h"
using namespace u8_literals;

ScriptTestEditor::ScriptTestEditor() 
{

}

ScriptTestEditor::~ScriptTestEditor() 
{

}

void ScriptTestEditor::OnStartGui() 
{
    SetLabel(u8"테스트 에디터"_c_str);
}

void ScriptTestEditor::OnPreFrame() 
{

}

void ScriptTestEditor::OnFrame() 
{
    using namespace Global;
    using namespace u8_literals;
    {
        auto& sceneMap = UmSceneManager.GetScenesMap();
        for (auto& [name, scene] : sceneMap)
        {
            ImGui::PushID(&scene);
            ImGui::Separator();
            std::string sceneName = scene.Name;
            ImGui::Text("Name : %s", sceneName.c_str());
            ImGui::Text("isLoaded : %s", scene.isLoaded ? "true" : "false");
            std::string scenePath = scene.Path;
            ImGui::Text("Path : %s", scenePath.c_str());
            if (ImGui::Button(u8"씬 저장"_c_str))
            {
                constexpr const char* TEST_SCENE_FOLDER = "Assets/Scenes";
                UmSceneManager.WriteSceneToFile(scene, TEST_SCENE_FOLDER, true);
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"씬 로드"_c_str))
            {
                UmSceneManager.LoadScene(scenePath);
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"씬 추가"_c_str))
            {
                UmSceneManager.LoadScene(scenePath, LoadSceneMode::ADDITIVE);
            }
            ImGui::SameLine();
            if (ImGui::Button(u8"씬 언로드"_c_str))
            {
                UmSceneManager.UnloadScene(scenePath);
            }
            ImGui::PopID();
        }
    }
}
