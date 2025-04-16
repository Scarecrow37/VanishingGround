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
    SetLabel(u8"스크립트 만들기"_c_str);
}

void ScriptTestEditor::OnPreFrame() 
{

}

void ScriptTestEditor::OnFrame() 
{
    using namespace Global;
    using namespace u8_literals;

    // 새 스크립트 파일 만들기 테스트용
    {
        static ImVec2      popupPos{};
        static std::string inputBuffer{};
        if (ImGui::Button(u8"테스트 스크립트 파일 만들기"_c_str))
        {
            popupPos = ImGui::GetMousePos();
            inputBuffer.clear();
            ImGui::OpenPopup(u8"스크립트 생성 팝업"_c_str);
        }

        if (ImGui::BeginPopup(u8"스크립트 생성 팝업"_c_str))
        {
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
            ImGui::Text(u8"컴포넌트 이름을 입력하세요."_c_str);
            ImGui::Text(u8"예) MyTest/MyFirstComponent"_c_str);
            ImGui::InputText("##new_script_file_name", &inputBuffer);
            if (ImGui::Button("OK"))
            {
                UmComponentFactory.MakeScriptFile(
                    inputBuffer.c_str());
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            if (ImGui::Button("Cancel"))
            {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    {
        constexpr const char* testPath = "TestObjectSerialized.yaml";
        ImGui::Text(u8"직렬화 테스트"_c_str);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad =
                    ImGui::AcceptDragDropPayload(DragDropTransform::key))
            {
                using Data      = DragDropTransform::Data;
                Data*      data = (Data*)payLoad->Data;
                YAML::Node node = UmGameObjectFactory.SerializeToYaml(
                    &data->pTransform->gameObject);

                std::ofstream ofs(testPath, std::ios::trunc);
                if (ofs.is_open())
                {
                    ofs << node;
                }
                ofs.close();
            }
            ImGui::EndDragDropTarget();
        }
        if (ImGui::Button(u8"역직렬화 테스트"_c_str))
        {
            YAML::Node node =  YAML::LoadFile(testPath);
            UmGameObjectFactory.DeserializeToYaml(&node);
        }
    }
}
