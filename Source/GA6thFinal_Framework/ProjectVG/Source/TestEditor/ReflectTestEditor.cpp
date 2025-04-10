#include "ReflectTestEditor.h"
using namespace u8_literals;

ReflectTestEditor::ReflectTestEditor() 
{

}

ReflectTestEditor::~ReflectTestEditor() 
{

}

void ReflectTestEditor::OnStartGui() 
{
    SetLabel(u8"헬로 월드"_c_str);
}

void ReflectTestEditor::OnPreFrame() 
{
    static MyTestClass testClass;

    ImGui::Begin("My Reflect Test");
    {
        testClass.ImGuiDrawPropertys();
    }
    ImGui::End();
}




