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
    static MyBaseClass testClass;
    
    MyFunc();
    
    ImGui::PushID(this);
    ImGui::Begin("My Reflect Test");
    {
        testClass.ImGuiDrawPropertys();        
    }
    ImGui::End();

    ImGui::Begin("My Reflect Test Setting");
    {
        static std::string saveData{};
        ImGui::Text(saveData.c_str());
        if (ImGui::Button("save"))
        {
            saveData = testClass.SerializedReflectFields();
        }
        ImGui::SameLine();
        if (ImGui::Button("Load"))
        {
            testClass.DeserializedReflectFields(saveData);
        }
    }
    ImGui::End();
    ImGui::PopID();
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

void MyBaseClass::SerializedReflectEvent() 
{
    std::memcpy(ReflectionFields->position.data(), &_position.x, sizeof(_position));
}

void MyBaseClass::DeserializedReflectEvent() 
{
    _position = Vector3(ReflectionFields->position.data());
}

void MyFunc()
{
    MyBaseClass base;

    base._position = Vector3{5.f, 5.f, 5.f};
    std::string serializedData = base.SerializedReflectFields(); //SerializedReflectEvent() 호출된 뒤 직렬화 반환함.
    base.DeserializedReflectFields(serializedData);              //역직렬화 이후 DeserializedReflectEvent() 호출.
}

