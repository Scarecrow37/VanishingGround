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
            std::string sceneName = scene.Name;
            ImGui::Text("Name : %s", sceneName.c_str());
            ImGui::Text("isLoaded : %s", scene.isLoaded ? "true" : "false");
            std::string scenePath = scene.Path;
            ImGui::Text("Path : %s", scenePath.c_str());
            ImGui::PopID();
        }
    }
}
