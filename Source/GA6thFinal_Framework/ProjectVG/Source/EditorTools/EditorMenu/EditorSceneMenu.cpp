#include "EditorSceneMenu.h"

using namespace Global;

void EditorSceneMenu::OnMenu() 
{
    const auto& objectKeys = EGameObjectFactory::Engine::GetGameObjectKeys();
    for (auto& i : objectKeys)
    {
        //ImGui::MenuItem();
    }
}
