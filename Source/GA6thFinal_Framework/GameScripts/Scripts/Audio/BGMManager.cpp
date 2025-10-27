#include "pchScripts.h"
#include "BGMManager.h"

UMREAL_COMPONENT(BGMManager)

BGMManager::BGMManager() = default;

BGMManager::~BGMManager()
{
    StopAllBGM();
}

void BGMManager::PlayBGM(const std::string& bgmKey, bool useFade)
{
    if (_currBGMKey != bgmKey)
    {
        _prevBGMHandle = _currBGMHandle;
        _prevBGMKey    = _currBGMKey;
        _currBGMKey    = bgmKey;
        _currBGMHandle = UmAudio.Play(bgmKey, Audio::GROUP_BGM, true);
        if (useFade)
        {
            _currBGMFader.Reset();
            _prevBGMFader.Reset();
        }
        else
        {
            UmAudio.Stop(_prevBGMHandle);
        }
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
    float currFactor = Volume * _currBGMFader.Fade();
    float prevFactor = Volume * _prevBGMFader.Fade();
    UmAudio.SetVolume(_currBGMHandle, currFactor);
    UmAudio.SetVolume(_prevBGMHandle, prevFactor);
}

void BGMManager::OnDestroy()
{
    StopAllBGM();
}

void BGMManager::ImGuiDrawPropertysEvent() {}

void BGMManager::SerializedReflectEvent() {}

void BGMManager::DeserializedReflectEvent() 
{
    _currBGMFader.SetFadeMode(Fader::FADE_IN);
    _currBGMFader.SetDuration(FadeDuration);
    _currBGMFader.SetFadeInType(Mathf::EaseType::EASE_IN, Mathf::EaseFuncType::SINE);

    _prevBGMFader.SetFadeMode(Fader::FADE_OUT);
    _prevBGMFader.SetDuration(FadeDuration);
    _prevBGMFader.SetFadeOutType(Mathf::EaseType::EASE_IN, Mathf::EaseFuncType::SINE);
    _prevBGMFader.SetOnFadeOutEndCallback([this]() { UmAudio.Stop(_prevBGMHandle); });
}
