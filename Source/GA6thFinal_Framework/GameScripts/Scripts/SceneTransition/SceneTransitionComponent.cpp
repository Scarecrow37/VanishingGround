#include "pchScripts.h"
#include "SceneTransitionComponent.h"
SceneTransitionComponent::SceneTransitionComponent() = default;

SceneTransitionComponent::~SceneTransitionComponent() = default;

void SceneTransitionComponent::ImGuiDrawPropertysEvent()
{
    ImGui::Text("Fade Start Color");
    ImGui::SameLine();
    bool result = ImGui::ColorEdit4("##Fade Start Color", (float*)&_startColor);

    ImGui::Text("Fade End Color");
    ImGui::SameLine();
    result = ImGui::ColorEdit4("##Fade End Color", (float*)&_endColor);

    bool isFadeButtonPressed = ImGui::Button("Fade", {100, 40});
    if (true == isFadeButtonPressed)
    {
        Fade(Duration, Maintain, _startColor, _endColor);
    }
}

void SceneTransitionComponent::SerializedReflectEvent()
{
    memcpy(ReflectFields->StartColorArray.data(), &_startColor, sizeof(Vector4));
    memcpy(ReflectFields->EndColorArray.data(), &_endColor, sizeof(Vector4));
}

void SceneTransitionComponent::DeserializedReflectEvent()
{
    memcpy(&_startColor,ReflectFields->StartColorArray.data(), sizeof(Vector4));
    memcpy(&_endColor, ReflectFields->EndColorArray.data(), sizeof(Vector4));
}

void SceneTransitionComponent::Fade(float duration, float maintain, const Vector4& start, const Vector4& end)
{
    UmTransition->Fade("Game", duration, start, end, maintain);
}
