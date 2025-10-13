#include "pch.h"
#include "EditorSceneMenu.h"

using namespace Global;
using namespace u8_literals;


void EditorSceneMenu::OnStartGui() 
{
    if (auto dock = editorModule->GetDockWindowSystem().GetDockWindow("Scene##dock"))
    {
        _sceneTool = dock->GetGui<EditorSceneTool>();
    }
}

void EditorSceneMenu::OnMenu()
{
    EditorModule& editor = *Global::editorModule;
    if (ImGui::BeginMenu("Scene"))
    {
        if (ImGui::MenuItem("New Scene"))
        {
            static std::string inputBuff;
            editor.OpenPopupBox(u8"씬 이름을 입력하세요"_c_str, [&]() 
            {
                ImGui::PushID(this);
                {              
                    ImGui::InputText(u8"##이름"_c_str, &inputBuff);
                    ImGui::Text(std::format("{}{}{}","SavePath : Scenes\\", inputBuff, ".UmScene").c_str());
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
        ImGui::MenuItem("Camera Setting", nullptr, &_isSceneCameraPopUp);
        ImGui::MenuItem("Sky box", nullptr, &_isSceneSkyBoxEditPopup);
        if(ImGui::BeginMenu("Gizmo"))
        {
            bool drawGizmo = _sceneTool->DrawGizmo;
            if (ImGui::MenuItem("Draw", nullptr, &drawGizmo))
            {
                _sceneTool->DrawGizmo = drawGizmo;
            }     
            ImGui::EndMenu();
        }
        ImGui::EndMenu();
    }

    SceneCameraPopUp();
    SceneSkyBoxEditPopup();
}

void EditorSceneMenu::SceneCameraPopUp() 
{
    if (_isSceneCameraPopUp)
    {
        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::Begin("Camera Setting", &_isSceneCameraPopUp, ImGuiWindowFlags_AlwaysAutoResize);
        _sceneTool->ImGuiDrawPropertys();
        _sceneTool->UpdateCameraSetting();
        _sceneTool->UpdateReflectFields();
        ImGui::End();
    }
}

void EditorSceneMenu::SceneSkyBoxEditPopup() 
{
    namespace fs = std::filesystem;
    if (_isSceneSkyBoxEditPopup)
    {
        ImGuiViewport* viewPort = ImGui::GetMainViewport();
        ImVec2 center = viewPort->GetCenter();
        ImVec2 size = viewPort->Size * 0.3f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(size, ImGuiCond_Appearing);
        ImGui::Begin("Skybox Setting", &_isSceneSkyBoxEditPopup);
        {
            size_t loadedSceneCount = UmSceneManager.LoadedSceneCount();
            if (0 < loadedSceneCount)
            {
                Scene* mainScene = UmSceneManager.GetMainScene();
                if (mainScene)
                {
                    ImGui::Text(u8"에셋 브라우저에서 hdr 파일을 드래그 드롭해 설정합니다."_c_str);
                    ImGui::Separator();
                    static std::string skyBoxBuffer = STR_NULL;
                    skyBoxBuffer = mainScene->_skyBox.ToPath().generic_string();
                    ImGui::InputText("ENV", &skyBoxBuffer, ImGuiInputTextFlags_ReadOnly);
                    // 에셋에 대한 드래그 앤 드롭 이벤트 처리
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                        {
                            DragDropAsset::Data* data      = (DragDropAsset::Data*)payload->Data;
                            const File::Path&    path      = data->GetPath();
                            fs::path             extension = path.extension();
                            if (extension == ".hdr")
                            {
                                UmSceneManager.SetSkyBox(data->GetGuid());
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }

                    static std::string skyIBLBuffer = STR_NULL;
                    skyIBLBuffer = mainScene->_skyIBL.ToPath().generic_string();
                    ImGui::InputText("IBL", &skyIBLBuffer, ImGuiInputTextFlags_ReadOnly);
                    // 에셋에 대한 드래그 앤 드롭 이벤트 처리
                    if (ImGui::BeginDragDropTarget())
                    {
                        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(DragDropAsset::KEY))
                        {
                            DragDropAsset::Data* data      = (DragDropAsset::Data*)payload->Data;
                            const File::Path&    path      = data->GetPath();
                            fs::path             extension = path.extension();
                            if (extension == ".hdr")
                            {
                                UmSceneManager.SetSkyIBL(data->GetGuid());
                            }
                        }
                        ImGui::EndDragDropTarget();
                    }
                }           
            }
            else
            {
                ImGui::Text(u8"씬을 로드 해주세요."_c_str);
            }
        }
        ImGui::End();
    }
}
