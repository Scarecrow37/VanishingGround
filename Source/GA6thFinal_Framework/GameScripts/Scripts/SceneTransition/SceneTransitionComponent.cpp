#include "pchScripts.h"
#include "SceneTransitionComponent.h"
SceneTransitionComponent::SceneTransitionComponent() = default;
SceneTransitionComponent::~SceneTransitionComponent() = default;

void SceneTransitionComponent::Update() {}

void SceneTransitionComponent::Start() {}

void SceneTransitionComponent::Reset() {}

void SceneTransitionComponent::ImGuiDrawPropertysEvent() 
{
    ImGui::Text("Fade Start Color");
    ImGui::SameLine();
    bool result = ImGui::ColorEdit4("##Fade Start Color", (float*)&_startColor);
    if (true == result)
        isDirty = result;

    ImGui::Text("Fade End Color");
    ImGui::SameLine();
    result = ImGui::ColorEdit4("##Fade End Color", (float*)&_endColor);
    if (true == result)
        isDirty = result;

    if (true == isDirty)
    {
        isDirty = false;
    }

    bool isFadeButtonPressed = ImGui::Button("Fade" ,{100, 40});
    if (true == isFadeButtonPressed)
    {
        Fade(Duration, _startColor, _endColor);
    }

}

void SceneTransitionComponent::SerializedReflectEvent() 
{
    ReflectFields->StartColorArray[0] = _startColor.x;
    ReflectFields->StartColorArray[1] = _startColor.y;
    ReflectFields->StartColorArray[2] = _startColor.z;
    ReflectFields->StartColorArray[3] = _startColor.w;

    ReflectFields->EndColorArray[0] = _endColor.x;
    ReflectFields->EndColorArray[1] = _endColor.y;
    ReflectFields->EndColorArray[2] = _endColor.z;
    ReflectFields->EndColorArray[3] = _endColor.w;

}

void SceneTransitionComponent::DeserializedReflectEvent()
{
    _startColor = Vector4(ReflectFields->StartColorArray[0], ReflectFields->StartColorArray[1],
                          ReflectFields->StartColorArray[2], ReflectFields->StartColorArray[3]);
    _endColor   = Vector4(ReflectFields->EndColorArray[0], ReflectFields->EndColorArray[1],
                          ReflectFields->EndColorArray[2], ReflectFields->EndColorArray[3]);
}

void SceneTransitionComponent::Fade(float duration, const Vector4& start, const Vector4& end) 
{
    UmTransition->Fade("Game", Duration, _startColor, _endColor);
    UmTransition->Fade("Editor", Duration, _startColor, _endColor);

}
