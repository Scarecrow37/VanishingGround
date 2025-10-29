#include "pchScripts.h"
#include "SceneTransitionComponent.h"

UMREAL_COMPONENT(SceneTransitionComponent)

SceneTransitionComponent::SceneTransitionComponent() = default;

SceneTransitionComponent::~SceneTransitionComponent() = default;

void SceneTransitionComponent::ImGuiDrawPropertysEvent()
{
    bool isFadeButtonPressed = ImGui::Button("Fade", {100, 40});
    if (true == isFadeButtonPressed)
    {
        if (false == ReflectFields->Ease)
        {
            Fade(Duration, _startColor, _endColor, nullptr);
        }
        else
        {
            Fade((Mathf::EaseType)ReflectFields->EaseType, (Mathf::EaseFuncType)ReflectFields->EaseFuncType, Duration,
                 _startColor, _endColor, nullptr);
        }
    }
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

    bool ispresetbuttonpressed = ImGui::Button("Add Preset", {100, 40});
    if (true == ispresetbuttonpressed)
    {
        AddFadePreset();
    }
    ImGui::SameLine();
    bool isdeletebutton = ImGui::Button("Delete Preset", {100, 40});
    if (true == isdeletebutton)
    {
        auto it = ReflectFields->FadePresets.find(_selectedName);
        if (it != ReflectFields->FadePresets.end())
        {
            ReflectFields->FadePresets.erase(it);
        }
        _selectedName = "-";
    }
    bool isfadepreset = ImGui::Button("Fade this Preset", {100, 40});
    if (true == isfadepreset)
    {
        if (_selectedName != "-")
        {
            Fade(_selectedName, nullptr);
        }
    }
    const char* presetname = _selectedName.c_str();
    if (ImGui::BeginCombo("##fade preset", presetname))
    {
        for (auto& [presetname, preset] : ReflectFields->FadePresets)
        {
            bool isSelected = _selectedName == presetname;
            if (ImGui::Selectable(presetname.c_str(), isSelected))
            {
                _selectedName = presetname;
            }
        }
        ImGui::EndCombo();
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
    if (false == UmCore->IsPlay())
    {
        CalculateFade();
    }
}

void SceneTransitionComponent::OnDrawDebugSelected()
{
    if (false == UmCore->IsPlay())
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

    if (_fadeElapsedTimer >= Duration)
    {
        if (_fadeEndFlag == true)
        {
            _fadeFlag = false;
            if (_fadeCallBackFunction && true == _callbackFlag)
            {
                _fadeCallBackFunction();
                _callbackFlag = false;
            }
        }
        else
        {
            UmTransition->Fade("Game", EndColor, true);
            _fadeEndFlag = true;
        }
        return;
    }
    _fadeElapsedTimer += UmTime.UnscaledDeltaTime();

    float step = _fadeElapsedTimer / Duration;
    if (true == ReflectFields->Ease)
    {
        step = Mathf::Ease((Mathf::EaseType)ReflectFields->EaseType, (Mathf::EaseFuncType)ReflectFields->EaseFuncType,
                           ReflectFields->EaseThreshold, step);
        _easeLog.push_back(step);
    }
    UmTransition->Fade("Game", Color::Lerp(StartColor, EndColor, step), true);
}

void SceneTransitionComponent::Awake()
{
    _singletonObject.TrySingleTon(true);
    _singletonComponent.TrySingleTon();
}

void SceneTransitionComponent::Fade(float duration, const Vector4& start, const Vector4& end,
                                    std::function<void()> callback)
{
    _easeLog.clear();
    if (0 != Duration && 0 != duration)
    {
        _fadeElapsedTimer           = 0;
        _fadeFlag                   = true;
        _callbackFlag               = true;
        Duration                    = duration;
        StartColor                  = start;
        EndColor                    = end;
        ReflectFields->EaseType     = Mathf::EaseType::EASE_IN;
        ReflectFields->EaseFuncType = Mathf::EaseFuncType::LINEAR;
        _fadeCallBackFunction       = callback;
        if (_startColor.w < _endColor.w)
        {
            _transitionLock = true;
        }
        else
        {
            _transitionLock = false;
        }
    }
}

void SceneTransitionComponent::Fade(Mathf::EaseType easetype, Mathf::EaseFuncType easefunctype, float duration,
                                    const Vector4& start, const Vector4& end, std::function<void()> callback)
{
    _easeLog.clear();
    if (0 != Duration && 0 != duration)
    {
        _fadeElapsedTimer           = 0;
        _fadeFlag                   = true;
        _callbackFlag               = true;
        Duration                    = duration;
        StartColor                  = start;
        EndColor                    = end;
        ReflectFields->EaseType     = easetype;
        ReflectFields->EaseFuncType = easefunctype;
        _fadeCallBackFunction       = callback;
        if (_startColor.w < _endColor.w)
        {
            _transitionLock = true;
        }
        else
        {
            _transitionLock = false;
        }
    }
}

void SceneTransitionComponent::Fade(std::string_view presetName, std::function<void(void)> callback)
{
    _easeLog.clear();
    auto& [start, end, easing, duration]      = ReflectFields->FadePresets[presetName.data()];
    auto& [easetype, easefunctype, threshold] = easing;
    if (0 != duration)
    {
        _fadeElapsedTimer            = 0;
        _fadeFlag                    = true;
        _callbackFlag                = true;
        Duration                     = duration;
        _startColor.x                = start[0];
        _startColor.y                = start[1];
        _startColor.z                = start[2];
        _startColor.w                = start[3];
        _endColor.x                  = end[0];
        _endColor.y                  = end[1];
        _endColor.z                  = end[2];
        _endColor.w                  = end[3];
        ReflectFields->Ease          = easefunctype != Mathf::EaseFuncType::LINEAR;
        ReflectFields->EaseType      = easetype;
        ReflectFields->EaseFuncType  = easefunctype;
        ReflectFields->EaseThreshold = threshold;
        _fadeCallBackFunction        = callback;
        if (_startColor.w < _endColor.w)
        {
            _transitionLock = true;
        }
        else
        {
            _transitionLock = false;
        }
    }
}

void SceneTransitionComponent::SetFadeCallback(std::function<void(void)> callback)
{
    _fadeCallBackFunction = callback;
}

void SceneTransitionComponent::AddFadePreset()
{
    std::string name = _currentPresetName;
    if (name.empty())
        name = "Preset_" + std::to_string(ReflectFields->FadePresets.size());

    const float threshold = std::clamp(ReflectFields->EaseThreshold, 0.0f, 1.0f);
    const float duration  = std::max(0.0f, ReflectFields->Duration);

    const EasingPreset easing{static_cast<Mathf::EaseType>(ReflectFields->EaseType),
                              static_cast<Mathf::EaseFuncType>(ReflectFields->EaseFuncType), threshold};

    const std::array<float, 4> start = {_startColor.x, _startColor.y, _startColor.z, _startColor.w};
    const std::array<float, 4> end   = {_endColor.x, _endColor.y, _endColor.z, _endColor.w};

    FadePreset preset{start, end, easing, duration};

    auto [it, inserted] = ReflectFields->FadePresets.try_emplace(name, std::move(preset));
    if (!inserted)
        it->second = std::move(preset);
}

bool SceneTransitionComponent::IsTransitioning() const
{
    return _fadeFlag;
}

void SceneTransitionComponent::SceneTransitionFade(std::string_view inPreset, std::string_view outPreset,
                                                   std::function<void(void)> callback)
{
    if (false == _transitionLock)
    {
        Fade(inPreset, [callback, outPreset, this]() {
            if (nullptr != callback)
            {
                callback();
            }
            Fade(outPreset, [this]() { });
        });
    }


}
