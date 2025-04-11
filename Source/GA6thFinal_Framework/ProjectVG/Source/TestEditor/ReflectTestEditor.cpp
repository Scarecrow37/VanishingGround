#include "ReflectTestEditor.h"
using namespace u8_literals;

void MyFunc();

ReflectTestEditor::ReflectTestEditor() 
{

}

ReflectTestEditor::~ReflectTestEditor() 
{

}

void ReflectTestEditor::OnStartGui() 
{
    SetLabel(u8"스크립트 만들기"_c_str);
}

void ReflectTestEditor::OnPreFrame() 
{

}

void ReflectTestEditor::OnFrame() 
{
    // 새 스크립트 파일 만들기 테스트용
    {
        static ImVec2      popupPos{};
        static std::string inputBuffer{};
        if (ImGui::Button(u8"테스트 스크립트 파일 만들기"_c_str))
        {
            popupPos = ImGui::GetMousePos();
            inputBuffer.clear();
            ImGui::OpenPopup(u8"파일 이름을 입력하세요."_c_str);
        }

        if (ImGui::BeginPopup(u8"파일 이름을 입력하세요."_c_str))
        {
            ImGui::SetNextWindowPos(popupPos, ImGuiCond_Appearing);
            ImGui::InputText("##new_script_file_name", &inputBuffer);
            if (ImGui::Button("OK"))
            {
                Global::engineCore->ComponentFactory.MakeScriptFile(
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
}
