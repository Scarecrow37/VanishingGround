#pragma once
#include "Utility/SingletonHelper.h"

class SceneTransitionComponent : public Component
{
    // easetype, easefunctype, threshold
    using EasingPreset = std::tuple<UINT, UINT, float>;
    // startcolor, endcolor, easingpreset, duration
    using FadePreset = std::tuple<std::array<float, 4>, std::array<float, 4>, EasingPreset, float>;

public:
    SceneTransitionComponent();
    virtual ~SceneTransitionComponent();

    USING_PROPERTY(SceneTransitionComponent);

public:
    REFLECT_PROPERTY(Duration, StartColor, EndColor, Easing, PresetName)

    GETTER(float, Duration) { return ReflectFields->Duration; }
    SETTER(float, Duration) { ReflectFields->Duration = value; }
    PROPERTY(Duration)

    GETTER(Color, StartColor) { return _startColor; }
    SETTER(Color, StartColor) { _startColor = value; }
    PROPERTY(StartColor)

    GETTER(Color, EndColor) { return _endColor; }
    SETTER(Color, EndColor) { _endColor = value; }
    PROPERTY(EndColor)

    GETTER(bool, Easing) { return ReflectFields->Ease; }
    SETTER(bool, Easing) { ReflectFields->Ease = value; }
    PROPERTY(Easing)

    GETTER(std::string, PresetName) { return _currentPresetName; }
    SETTER(std::string, PresetName) { _currentPresetName = value; }
    PROPERTY(PresetName)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    std::array<float, 4>                        StartColorArray;
    std::array<float, 4>                        EndColorArray;
    float                                       Duration;
    bool                                        Ease;
    UINT                                        EaseType      = 0;
    UINT                                        EaseFuncType  = 0;
    float                                       EaseThreshold = 0.5f;
    std::unordered_map<std::string, FadePreset> FadePresets;
    REFLECT_FIELDS_END(SceneTransitionComponent)

    Color              _startColor;
    Color              _endColor;
    float              _fadeElapsedTimer = 0.f;
    bool               _fadeFlag         = false;
    std::vector<float> _easeLog;
    std::string        _currentPresetName = "";
    std::string        _selectedName      = "-";

    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;
    void OnDrawDebug() override;
    void OnDrawDebugSelected() override;
    void Update() override;
    void CalculateFade();

    // void Reset() override;

     void Awake() override;

    // void OnDestroy() override;

public:
    void Fade(float duration, const Vector4& start, const Vector4& end, std::function<void()> callback);
    void Fade(Mathf::EaseType easetype, Mathf::EaseFuncType easefunctype, float duration, const Vector4& start,
              const Vector4& end, std::function<void()> callback);
    void Fade(std::string_view presetName, std::function<void(void)> callback);
    void SetFadeCallback(std::function<void(void)> callback);
    void AddFadePreset();
    bool IsTransitioning() const;
    void SceneTransitionFade(std::string_view inPreset, std::string_view outPreset, std::function<void(void)> callback);



private:
    SingletonComponent<SceneTransitionComponent> _singletonComponent{this};
    SingletonObject<SceneTransitionComponent>    _singletonObject{this};

    bool                      _callbackFlag = true;
    std::function<void(void)> _fadeCallBackFunction;
    bool                      _transitionLock = false;
};
