#include "pchScripts.h"
#include "ModelTest.h"

IntToString::IntToString(MVVM::Model<int>& model) : MVVM::ViewModel<int, std::string>(model)
{
}

std::string IntToString::Convert(const int& value)
{
    return std::to_string(value);
}

ModelTest::ModelTest() = default;
ModelTest::~ModelTest() = default;

void ModelTest::Reset()
{
    Component::Reset();
    UmWatcher.Register<IntToString>("TestValue", _value);
}

void ModelTest::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();
    if (ImGui::Button("Decrease"))
    {
        _value.Set(_value.Get() - 1);
    }
    ImGui::SameLine();
    if (ImGui::Button("Increase"))
    {
        _value.Set(_value.Get() + 1);
    }
}

void ViewTest::Reset()
{
    TextElement::Reset();
    UmTime.Invoke(this, 0, [this] {
        UmWatcher.Watch<IntToString, std::string>("TestValue", [this](const std::string& value) { Text = value; });
    });

}