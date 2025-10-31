#include "pchScripts.h"
#include "BGMComponent.h"
#include <Audio/BGMManager.h>

UMREAL_COMPONENT(BGMComponent)

BGMComponent::BGMComponent() = default;

BGMComponent::~BGMComponent() = default;

void BGMComponent::Start() 
{
    if (BGMManager* bgmManager = SingletonComponent<BGMManager>::GetInstance())
    {
        std::string bgmKey = std::to_string(ReflectFields->AudioID);
        bgmManager->PlayBGM(bgmKey, ReflectFields->UseFade);
    }
}
