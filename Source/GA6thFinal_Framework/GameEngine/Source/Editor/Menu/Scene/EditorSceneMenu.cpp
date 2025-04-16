#include "pch.h"
#include "EditorSceneMenu.h"

using namespace Global;

void EditorSceneMenuGameObject::OnMenu() 
{
    const auto& objectKeys = EGameObjectFactory::Engine::GetGameObjectKeys();
    for (auto& key : objectKeys)
    {
        const char* name = key.c_str() + 6;
        const std::string& labal = std::format("New {}", name);
        if (ImGui::MenuItem(labal.c_str()))
        {
            engineCore->GameObjectFactory.NewGameObject(key, 
                GameObject::Helper::GenerateUniqueName(name));
        }
    }
}

void EditorSceneMenuScenes::OnMenu() 
{
    if(ImGui::MenuItem("New Empty Scene"))
    {
        engineCore->SceneManager.LoadScene("Empty Scene");
    }
}
