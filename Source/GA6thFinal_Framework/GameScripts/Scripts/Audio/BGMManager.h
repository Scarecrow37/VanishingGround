#pragma once
#include <Utility/SingletonHelper.h>
#include <Utility/FadeHelper.h>

class BGMManager : public Component
{
    USING_PROPERTY(BGMManager)

public:
    BGMManager();
    ~BGMManager() override;

public:
    REFLECT_PROPERTY(Volume, CurrentFadeFactor, PreviousFadeFactor)

    GETTER(float, Volume) { return ReflectFields->Volume; }
    SETTER(float, Volume) { ReflectFields->Volume = std::clamp(value, 0.0f, 1.0f); }
    PROPERTY(Volume)

    GETTER_ONLY(float, CurrentFadeFactor) { return _currBGMFader.GetFadeFactor(); }
    PROPERTY(CurrentFadeFactor)

    GETTER_ONLY(float, PreviousFadeFactor) { return _prevBGMFader.GetFadeFactor(); }
    PROPERTY(PreviousFadeFactor)

public:
    void PlayBGM(const std::string& bgmKey, bool useFade = true);
    void StopAllBGM();

protected:
    void Awake() override;
    void Update() override;
    void OnDestroy() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

private:
    SingletonObject<BGMManager> _singletonObject{this};
    std::string _currBGMKey;
    std::string _prevBGMKey;
    Audio::AudioHandle _currBGMHandle;
    Audio::AudioHandle _prevBGMHandle;
    Fader _currBGMFader;
    Fader _prevBGMFader;

    inline static constexpr float FADE_DURATION = 1.0f;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float Volume = 1.0f;
    REFLECT_FIELDS_END(BGMManager)
};