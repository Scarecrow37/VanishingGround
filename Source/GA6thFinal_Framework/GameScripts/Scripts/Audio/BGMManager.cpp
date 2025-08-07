#include "pchScripts.h"
#include "BGMManager.h"
#include <Audio/AudioComponent.h>

BGMManager::BGMManager() = default;
BGMManager::~BGMManager()
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
    if (_audio)
    {
        _audio->Stop();
    }
}

void BGMManager::Reset() 
{
    _staticInstance = this;
}

void BGMManager::Start()
{
    _audio = gameObject->GetComponent<AudioComponent>();
    if (_audio)
    {
        _audio->Play();
    }
}

void BGMManager::PlayBGM(const std::string& bgmKey) 
{
    if (_audio)
    {
        //_audio->Play(bgmKey);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "AudioTableComponent is not found in BGMManager.");
    }
}

void BGMManager::StopBGM(const std::string& bgmKey) 
{
    //if (_audioTable)
    //{
    //    //_audioTable->Stop(bgmKey);
    //}
    //else
    //{
    //    UmLogger.Log(LogLevel::LEVEL_ERROR, "AudioTableComponent is not found in BGMManager.");
    //}
}

