#include "pch.h"
#include "EditorSceneMenu.h"

using namespace Global;
using namespace u8_literals;


void EditorSceneMenuScenes::OnMenu()
{
    EditorModule& editor = *Global::editorModule;
    if (ImGui::BeginMenu("Scene"))
    {
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
                            if (inputBuff.empty() == false)
                            {
                                std::filesystem::path outPath = "Scenes";
                                UmSceneManager.WriteEmptySceneToFile(inputBuff, outPath.string());
                                ImGui::CloseCurrentPopup();
                            }
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
        if (ImGui::MenuItem("Camera Setting"))
        {
            _isSceneCameraPopUp = true;
        }
        ImGui::EndMenu();
    }

    SceneCameraPopUp();
}

void EditorSceneMenuScenes::SceneCameraPopUp() 
{
    if (nullptr == _sceneTool)
    {
        _sceneTool = editorModule->GetDockWindowSystem().GetDockWindow("SceneDock")->GetGui<EditorSceneTool>();
    }
    else if(_isSceneCameraPopUp)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Camera Setting", &_isSceneCameraPopUp, ImGuiWindowFlags_AlwaysAutoResize);
        _sceneTool->ImGuiDrawPropertys();
        _sceneTool->UpdateCameraSetting();
        ImGui::End();
    }
}
