#include "pchScripts.h"
#include "BGMComponent.h"
#include <Audio/BGMManager.h>

UMREAL_COMPONENT(BGMComponent)

BGMComponent::BGMComponent() = default;

BGMComponent::~BGMComponent() = default;

void BGMComponent::Start() 
{
    PlayBGM(ReflectFields->AudioID, false);
}
void BGMComponent::OnDestroy() 
{
    if (BGMManager* bgmManager = SingletonComponent<BGMManager>::GetInstance())
    {
        bgmManager->SetCurrentBGMSleep();
    }
}
void BGMComponent::PlayBGM(int id, bool useFade)
{
    if (BGMManager* bgmManager = SingletonComponent<BGMManager>::GetInstance())
    {
        std::string bgmKey = std::to_string(id);
        bgmManager->PlayBGM(bgmKey, useFade);
    }
}
void BGMComponent::PlayBGM(const std::string& bgmKey, bool useFade)
{
    if (BGMManager* bgmManager = SingletonComponent<BGMManager>::GetInstance())
    {
        bgmManager->PlayBGM(bgmKey, useFade);
    }
}
