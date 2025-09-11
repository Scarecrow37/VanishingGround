#pragma once
#include "UmFramework.h"
#include "Utility/SingletonHelper.h"

class AudioComponent;
class BGMManager : public Component
{
    USING_PROPERTY(BGMManager)

public:
    BGMManager();
    ~BGMManager() override;

public:
    REFLECT_PROPERTY()

public:
    void Reset() override;
    void Start() override;

public:
    void PlayBGM(const std::string& bgmKey);
    void StopBGM(const std::string& bgmKey);

protected:
    AudioComponent* _audio = nullptr;
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(BGMManager)

    SingletonComponent<BGMManager> _singletonComponent{this};
};
