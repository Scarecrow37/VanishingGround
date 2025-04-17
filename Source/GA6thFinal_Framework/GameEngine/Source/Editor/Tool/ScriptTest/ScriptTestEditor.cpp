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
    {
        constexpr const char* testPath = "TestObjectSerialized.yaml";
        ImGui::TextColored({0,0,1,1}, u8"직렬화 테스트"_c_str);
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

        ImGui::TextColored({0,0,1,1}, u8"Instantiate"_c_str);
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payLoad = ImGui::AcceptDragDropPayload(DragDropTransform::key))
            {
                using Data      = DragDropTransform::Data;
                Data*      data = (Data*)payLoad->Data;
                GameObject::Instantiate(data->pTransform->gameObject);
            }
            ImGui::EndDragDropTarget();
        }
    }
}
