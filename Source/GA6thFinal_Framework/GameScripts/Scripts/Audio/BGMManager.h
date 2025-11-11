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
    REFLECT_PROPERTY(CurrentAudioKey, PreviousAudioKey, SleepAudioKey, Volume, FadeDuration, CurrentFadeFactor,
                     PreviousFadeFactor)

    GETTER_ONLY(std::string_view, CurrentAudioKey) { return _currBGMKey; }
    PROPERTY(CurrentAudioKey)

    GETTER_ONLY(std::string_view, PreviousAudioKey) { return _prevBGMKey; }
    PROPERTY(PreviousAudioKey)

    GETTER_ONLY(std::string_view, SleepAudioKey) { return _sleepBGMKey; }
    PROPERTY(SleepAudioKey)

    GETTER(float, Volume) { return ReflectFields->Volume; }
    SETTER(float, Volume)
    {
        ReflectFields->Volume = std::clamp(value, 0.0f, 1.0f);
        UpdateVolume();
    }
    PROPERTY(Volume)

    GETTER(float, FadeDuration) { return ReflectFields->FadeDuration; }
    SETTER(float, FadeDuration) { ReflectFields->FadeDuration = std::max(value, 0.0f); }
    PROPERTY(FadeDuration)

    GETTER_ONLY(float, CurrentFadeFactor) { return _currBGMFader.GetFadeFactor(); }
    PROPERTY(CurrentFadeFactor)

    GETTER_ONLY(float, PreviousFadeFactor) { return _prevBGMFader.GetFadeFactor(); }
    PROPERTY(PreviousFadeFactor)

public:
    void PlayBGM(const std::string& bgmKey, bool useFade = true);
    void StopAllBGM();

    /// <summary>
    /// 현재 BGM을 Wait상태에 둔 후, 다음 Play(씬이 넘어가는 등)때 재생 요청 시, 같은 BGM이라면 이어서 재생합니다.
    /// </summary>
    void SetCurrentBGMSleep();

private:
    void Awake() override;
    void Update() override;
    void OnDestroy() override;
    void ImGuiDrawPropertysEvent() override;
    void SerializedReflectEvent() override;
    void DeserializedReflectEvent() override;

    void UpdateVolume();

private:
    SingletonObject<BGMManager>     _singletonObject{this};
    SingletonComponent<BGMManager>  _singletonComponent{this};
    std::string _currBGMKey;
    std::string _prevBGMKey;
    Audio::AudioHandle _currBGMHandle;
    Audio::AudioHandle _prevBGMHandle;
    Fader _currBGMFader;
    Fader _prevBGMFader;

    std::string        _sleepBGMKey;
    Audio::AudioHandle _sleepBGMHandle;

    inline static constexpr float FADE_DURATION = 1.0f;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    float Volume = 1.0f;
    float FadeDuration = 1.0f;
    REFLECT_FIELDS_END(BGMManager)
};