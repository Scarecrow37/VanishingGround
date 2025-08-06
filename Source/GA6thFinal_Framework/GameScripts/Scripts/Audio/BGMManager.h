#pragma once
#include "UmFramework.h"

class AudioTableComponent;
class BGMManager : public Component
{
    USING_PROPERTY(BGMManager)

public:
    BGMManager();
    ~BGMManager() override;
    inline static BGMManager* _staticInstance;
    inline static BGMManager* GetInstance() { return _staticInstance; }

public:
    REFLECT_PROPERTY()

public:
    void Reset() override;
    void Awake() override;

public:
    void PlayBGM(const std::string& bgmKey);
    void StopBGM(const std::string& bgmKey);

protected:
    AudioTableComponent* _audioTable = nullptr;
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(BGMManager)
};
