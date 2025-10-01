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

    float _volume = 1.0f;
    Fader _currBGMFader;
    Fader _prevBGMFader;

    inline static constexpr float FADE_DURATION = 1.0f;

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(BGMManager)
};