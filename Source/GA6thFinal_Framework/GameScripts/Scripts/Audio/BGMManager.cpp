#include "pchScripts.h"
#include "BGMManager.h"

UMREAL_COMPONENT(BGMManager)

BGMManager::BGMManager()
{
    _currBGMFader.SetFadeInType(Mathf::EaseType::EASE_IN, Mathf::EaseFuncType::SINE);
    _currBGMFader.SetDuration(FADE_DURATION);

    _prevBGMFader.SetFadeInType(Mathf::EaseType::EASE_OUT, Mathf::EaseFuncType::SINE);
    _prevBGMFader.SetDuration(FADE_DURATION);
    _prevBGMFader.SetOnFadeInEndCallback([this]() {
        UmAudio.Stop(_prevBGMHandle);
        _prevBGMFader.SetFadeMode(Fader::FADE_NONE);
        _prevBGMFader.SetTimer(0.0f);
    });
}

BGMManager::~BGMManager()
{
    StopAllBGM();
}

void BGMManager::PlayBGM(const std::string& bgmKey, bool useFade)
{
    // 혹시 모르니 이전 오디오는 해제
    UmAudio.Stop(_prevBGMHandle);

    _prevBGMHandle = _currBGMHandle;
    _prevBGMKey    = _currBGMKey;
    _currBGMKey    = bgmKey;
    _currBGMHandle = UmAudio.Play(bgmKey, Audio::GROUP_BGM, true);
    if (useFade)
    {
        _currBGMFader.SetTimer(0.0f);
        _prevBGMFader.SetTimer(0.0f);
        _currBGMFader.SetFadeMode(Fader::FADE_IN);
        _prevBGMFader.SetFadeMode(Fader::FADE_IN);
    }
    else
    {
        UmAudio.Stop(_prevBGMHandle);
        _currBGMFader.SetTimer(FADE_DURATION);
        _prevBGMFader.SetTimer(FADE_DURATION);
        _currBGMFader.SetFadeMode(Fader::FADE_NONE);
        _prevBGMFader.SetFadeMode(Fader::FADE_NONE);
    }
}

void BGMManager::StopAllBGM()
{
    UmAudio.Stop(_currBGMHandle);
    UmAudio.Stop(_prevBGMHandle);
}

void BGMManager::Awake() 
{
    _singletonObject.TrySingleTon(true);
}

void BGMManager::Update()
{
    float currFactor = _currBGMFader.Fade();
    float prevFactor = _prevBGMFader.Fade();
    UmAudio.SetVolume(_currBGMHandle, _volume * currFactor);
    UmAudio.SetVolume(_prevBGMHandle, _volume * (1.0f - prevFactor));
}

void BGMManager::OnDestroy()
{
    StopAllBGM();
}

void BGMManager::ImGuiDrawPropertysEvent() {}

void BGMManager::SerializedReflectEvent() {}

void BGMManager::DeserializedReflectEvent() {}
