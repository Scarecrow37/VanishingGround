#include "pchScripts.h"
#include "BGMComponent.h"
#include <Audio/BGMManager.h>

UMREAL_COMPONENT(BGMComponent)

BGMComponent::BGMComponent() = default;

BGMComponent::~BGMComponent() = default;

void BGMComponent::Start() 
{
    if (GameObject* object = SingletonObject<BGMManager>::GetInstance())
    {
        if (BGMManager* manager = object->GetComponent<BGMManager>())
        {
            std::string bgmKey = std::to_string(ReflectFields->AudioID);
            manager->PlayBGM(bgmKey, ReflectFields->UseFade);
        }
    }
}
