#include "pchScripts.h"
#include "ArtifactFrameUIManager.h"

ArtifactFrameUIManager::ArtifactFrameUIManager()
{
    static_instance = this;
}

ArtifactFrameUIManager::~ArtifactFrameUIManager()
{
    if (this == static_instance)
    {
        static_instance = nullptr;
    }
}

void ArtifactFrameUIManager::Reset() 
{
    Base::Reset();
}

void ArtifactFrameUIManager::Awake() 
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
