#include "pchScripts.h"
#include "SceneTransitionComponent.h"
SceneTransitionComponent::SceneTransitionComponent() = default;

SceneTransitionComponent::~SceneTransitionComponent() = default;

void SceneTransitionComponent::ImGuiDrawPropertysEvent()
{
    if (true == ReflectFields->Ease)
    {
        const char* easetype = Mathf::EaseNameTable[ReflectFields->EaseType].c_str();
        if (ImGui::BeginCombo("##ease type", easetype))
        {
            for (int i = 0; i < Mathf::EaseNameTable.size(); ++i)
            {
                bool isSelected = ReflectFields->EaseType == i;
                if (ImGui::Selectable(Mathf::EaseNameTable[i].c_str(), isSelected))
                {
                    ReflectFields->EaseType = i;
                }
            }
            ImGui::EndCombo();
        }
        const char* easefunc = Mathf::EaseFuncNameTable[ReflectFields->EaseFuncType].c_str();
        if (ImGui::BeginCombo("##ease func type", easefunc))
        {
            for (int i = 0; i < Mathf::EaseFuncNameTable.size(); ++i)
            {
                bool isSelected = ReflectFields->EaseFuncType == i;
                if (ImGui::Selectable(Mathf::EaseFuncNameTable[i].c_str(), isSelected))
                {
                    ReflectFields->EaseFuncType = i;
                }
            }
            ImGui::EndCombo();
        }
        
        ImGui::SliderFloat("Shift Threshold", &ReflectFields->EaseThreshold, 0.f, 1.f);
        ImGui::PlotLines("Ease Graph", _easeLog.data(), (int)_easeLog.size(), 0, NULL, -0.5f, 1.5f, ImVec2(400, 150));
    }

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
    memcpy(&_startColor, ReflectFields->StartColorArray.data(), sizeof(Vector4));
    memcpy(&_endColor, ReflectFields->EndColorArray.data(), sizeof(Vector4));
}

void SceneTransitionComponent::OnDrawDebug()
{
    if (false == Global::IsPlay())
    {
        CalculateFade();
    }
}

void SceneTransitionComponent::OnDrawDebugSelected()
{
    if (false == Global::IsPlay())
    {
        CalculateFade();
    }
}

void SceneTransitionComponent::Update()
{
    CalculateFade();
}

void SceneTransitionComponent::CalculateFade() 
{

    if (false == _fadeFlag)
    {
        return;
    }
    _fadeElapsedTimer += UmTime.DeltaTime();

    if (_fadeElapsedTimer >= Duration+Maintain)
    {
        _fadeFlag         = false;
        _fadeElapsedTimer = 0;
        UmTransition->Fade("Game", {0,0,0,0}, false);
        return;
    }
    float step         = _fadeElapsedTimer / Duration;
    if (true == ReflectFields->Ease)
    {
        step = Mathf::Ease((Mathf::EaseType)ReflectFields->EaseType, (Mathf::EaseFuncType)ReflectFields->EaseFuncType, ReflectFields->EaseThreshold, step);
        _easeLog.push_back(step);
    }
    UmTransition->Fade("Game", Color::Lerp(StartColor, EndColor, step), true);
}

void SceneTransitionComponent::Fade(float duration, float maintain, const Vector4& start, const Vector4& end)
{
    _easeLog.clear();
    if (0 != Duration && 0!= duration)
    {
        _fadeFlag = true;
        Duration  = duration;
        Maintain  = maintain;
        StartColor = start;
        EndColor   = end;
    }
}
