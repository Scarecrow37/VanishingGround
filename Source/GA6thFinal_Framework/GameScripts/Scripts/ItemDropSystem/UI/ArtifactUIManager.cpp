#include "pchScripts.h"
#include "ArtifactUIManager.h"

ArtifactUIManager::ArtifactUIManager()
{
    static_instance = this;
}

ArtifactUIManager::~ArtifactUIManager()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void ArtifactUIManager::Reset() 
{
    Base::Reset();
}

void ArtifactUIManager::Awake() 
{
    if (this != static_instance)
    {
        UmLogger.Log(LogLevel::LEVEL_WARNING, u8"ArtifactFrameUIManager는 하나만 존재해야 합니다.");
        GameObject::Destroy(this);
    }
    else
    {
        gameObject->AddTag(TAG);
        Base::Awake();
    }
}
