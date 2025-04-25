#include "pch.h"
#include "EditorSceneMenu.h"

using namespace Global;
using namespace u8_literals;

void EditorSceneMenuScenes::OnMenu() 
{
    EditorModule& editor = *Global::editorModule;
    if (ImGui::BeginMenu("New Scene File"))
    {
        if (ImGui::MenuItem("New EmptyScene"))
        {
            static std::string inputBuff;
            editor.OpenPopupBox(u8"씬 이름을 입력하세요"_c_str, [&]() {
                ImGui::PushID(this);
                {
                    ImGui::InputText(u8"이름"_c_str, &inputBuff);
                    if (ImGui::Button(u8"확인"_c_str))
                    {
                        std::filesystem::path outPath = "Scenes";
                        UmSceneManager.WriteEmptySceneToFile(inputBuff, outPath.string());
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    if (ImGui::Button(u8"취소"_c_str))
                    {
                        ImGui::CloseCurrentPopup();
                    }
                }
                ImGui::PopID();
            });
        }
        ImGui::EndMenu();
    }
}

void EditorSceneMenuGameObject::OnMenu()
{
    if (ImGui::BeginMenu("New GameObject"))
    {
        const auto& objectKeys = EGameObjectFactory::Engine::GetGameObjectKeys();
        for (auto& key : objectKeys)
        {
            const char* name  = key.c_str() + 6;
            if (ImGui::MenuItem(name))
            {
                UmGameObjectFactory.NewGameObject(key, GameObject::Helper::GenerateUniqueName(name));
            }
        }
        ImGui::EndMenu();
    }
}