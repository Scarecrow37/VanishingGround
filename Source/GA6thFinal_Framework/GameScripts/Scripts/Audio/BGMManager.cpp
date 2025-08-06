#include "pchScripts.h"
#include "BGMManager.h"
#include <Audio/Table/AudioTableComponent.h>

BGMManager::BGMManager() = default;
BGMManager::~BGMManager()
{
    if (this == _staticInstance)
    {
        _staticInstance = nullptr;
    }
}

void BGMManager::Reset() 
{
    _staticInstance = this;
    _audioTable     = gameObject->GetComponent<AudioTableComponent>();
}

void BGMManager::Awake()
{
    if (_audioTable)
    {
        _audioTable->Play("BGM_Battle0");
    }
}

void BGMManager::PlayBGM(const std::string& bgmKey) 
{
    if (_audioTable)
    {
        _audioTable->Play(bgmKey);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "AudioTableComponent is not found in BGMManager.");
    }
}

void BGMManager::StopBGM(const std::string& bgmKey) 
{
    if (_audioTable)
    {
        //_audioTable->Stop(bgmKey);
    }
    else
    {
        UmLogger.Log(LogLevel::LEVEL_ERROR, "AudioTableComponent is not found in BGMManager.");
    }
}

