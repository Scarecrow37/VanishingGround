#include "pchScripts.h"
#include "GameOverManager.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(GameOverManager)

void GameOverManager::Awake() 
{
    _singletoneComponent.TrySingleTon();
    BindInputAction(ControllerButton::A, Action::PRESSED, this, &GameOverManager::PressedAButton);
}

void GameOverManager::Start()
{
    _vanishedOverlay     = gameObject->GetComponent<OverlayPanel>();
    const int childCount = gameObject->transform->GetChildCount();
    for (int i = 0; i < childCount; ++i)
    {
        if (Transform* child = gameObject->transform->GetChild(i))
        {
            if (child->gameObject->CompareTag("Vanished Animation"))
            {
                _vanishedAnimation                         = child->gameObject->GetComponent<SpriteAnimationElement>();
                _vanishedAnimation->gameObject->ActiveSelf = false;
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
            TransitionTitleScene();
        }
    }
}

void GameOverManager::ProcessGameOver() 
{
    if (false == _isBeginProcess)
    {
        if (_vanishedAnimation)
        {
            _vanishedAnimation->gameObject->ActiveSelf = true;
            _vanishedAnimation->Setup();
            _vanishedAnimation->StartAnimation();
            _isBeginProcess = true;

            PushInputLayer();
        }
        else
        {
            TransitionTitleScene();
        }
    }
}

void GameOverManager::TransitionTitleScene()
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
                _isBeginProcess = false;

                PopInputLayer();
            }
        });
    }
}

void GameOverManager::PressedAButton(const Input::Controller& controller) 
{
}
