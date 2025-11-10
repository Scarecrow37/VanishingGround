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
    // Sleep 상태의 상태를 현재 상태로 변경
    if (false == _sleepBGMKey.empty())
    {
        _currBGMKey    = _sleepBGMKey;
        _currBGMHandle = _sleepBGMHandle;
    }
    _sleepBGMKey.clear();
    _sleepBGMHandle = Audio::AudioHandle{};

    if (_currBGMKey != bgmKey)
    {
        // 이전 키
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
    UpdateVolume();
}


void BGMManager::StopAllBGM()
{
    UmAudio.Stop(_currBGMHandle);
    UmAudio.Stop(_prevBGMHandle);
    UmAudio.Stop(_sleepBGMHandle);
}

void BGMManager::SetCurrentBGMSleep()
{
    _sleepBGMKey    = _currBGMKey;
    _sleepBGMHandle = _currBGMHandle;
    _currBGMKey     = "";
    _currBGMHandle  = Audio::AudioHandle{};
    UmAudio.SetVolume(_sleepBGMHandle, 0.0f);
}

void BGMManager::Awake()
{
    _singletonObject.TrySingleTon(true);
    _singletonComponent.TrySingleTon();
}

void BGMManager::Update()
{
    if (false == _currBGMKey.empty())
    {
        _currBGMFader.Fade();
    }
    if (false == _prevBGMKey.empty())
    {
        _prevBGMFader.Fade();
    }
    UpdateVolume();
}

void BGMManager::OnDestroy()
{
    StopAllBGM();
}

void BGMManager::ImGuiDrawPropertysEvent() {}

void BGMManager::SerializedReflectEvent() {}

void BGMManager::DeserializedReflectEvent() 
{
    // DeltaTime을 쓰지 않도록 할 것
    _currBGMFader.UseUnScaledDeltaTime(false);
    _prevBGMFader.UseUnScaledDeltaTime(false);

    _currBGMFader.SetFadeMode(Fader::FADE_IN);
    _currBGMFader.SetDuration(FadeDuration);
    _currBGMFader.SetFadeInType(Mathf::EaseType::EASE_IN, Mathf::EaseFuncType::SINE);

    _prevBGMFader.SetFadeMode(Fader::FADE_OUT);
    _prevBGMFader.SetDuration(FadeDuration);
    _prevBGMFader.SetFadeOutType(Mathf::EaseType::EASE_IN, Mathf::EaseFuncType::SINE);
    _prevBGMFader.SetOnFadeOutEndCallback([this]() { UmAudio.Stop(_prevBGMHandle); });
}

void BGMManager::UpdateVolume() 
{
    if (false == _currBGMKey.empty())
    {
        float currFactor = ReflectFields->Volume * _currBGMFader.GetFadeFactor();
        UmAudio.SetVolume(_currBGMHandle, currFactor);
    }
    if (false == _prevBGMKey.empty())
    {
        float prevFactor = ReflectFields->Volume * _prevBGMFader.GetFadeFactor();
        UmAudio.SetVolume(_prevBGMHandle, prevFactor);
    }
}
