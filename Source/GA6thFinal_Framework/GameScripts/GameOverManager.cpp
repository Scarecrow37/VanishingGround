#include "pchScripts.h"
#include "GameOverManager.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(GameOverManager)

void GameOverManager::Start() 
{
    if (auto object = GameObject::FindWithTag("Vanished Panel").lock())
    {
        _vanishedOverlay = object->GetComponent<OverlayPanel>();
        const int childCount = object->transform->GetChildCount();
        for (int i = 0; i < childCount; ++i)
        {
            if (Transform* child = object->transform->GetChild(i))
            {
                if (child->gameObject->CompareTag("Vanished Animation"))
                {
                    _vanishedAnimation = child->gameObject->GetComponent<SpriteAnimationElement>();
                }
            }
        }
    }
}

void GameOverManager::Update() 
{
    if (_isBeginProcess)
    {
        if (false == _vanishedAnimation->IsPlaying)
        {
            if (auto* sceneTransition = SingletonComponent<SceneTransitionComponent>::GetInstance())
            {
                std::weak_ptr<GameObject> weakOwner = gameObject->GetWeakPtr();
                sceneTransition->SceneTransitionFade("in", "out", [this, weakOwner]() {
                    GameObject* owner = weakOwner.lock().get();
                    assert(owner && "콜백으로 등록한 객체가 댕글링 포인터입니다.");
                    if (owner)
                    {
                        UmSceneManager.LoadScene(UmFileSystem.GetPathFromGuid("cd798e18-e5fd-421b-9b23-ef7bcfab15a0").string());
                    }
                });
            }
        }
    }
}

void GameOverManager::ProcessGameOver() 
{
    if (false == _isBeginProcess)
    {
        if (_vanishedOverlay)
        {
            _vanishedOverlay->gameObject->ActiveSelf = true;
        }
        if (_vanishedAnimation)
        {
            _vanishedAnimation->Setup();
            _vanishedAnimation->StartAnimation();
        }
        _isBeginProcess = true;
    }
}