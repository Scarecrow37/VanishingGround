#include "pchScripts.h"
#include "GameOverManager.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "SceneTransition/SceneTransitionComponent.h"
#include "Utility/SceneGuid.h"
#include "CombatUIManager/CombatUIManager.h"
#include "QTE/UI/QTEUIManager.h"

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
                _vanishedAnimation = child->gameObject->GetComponent<SpriteAnimationElement>();
               
            }
            if (child->gameObject->CompareTag("Vanished Background"))
            {
                _vanishedBackground = child->gameObject->GetComponent<ImageElement>();
            }
        }
    }
    if (_vanishedAnimation)
    {
        _vanishedAnimation->gameObject->ActiveSelf = false;
    }
    if (_vanishedBackground)
    {
        _vanishedBackground->Alpha                  = 0.0f;
        _vanishedBackground->gameObject->ActiveSelf = false;
    }
    _backgroundFader.SetDuration(1.0f);
    _backgroundFader.SetFadeMode(Fader::FADE_IN);
    _backgroundFader.SetFadeInType(Mathf::EASE_OUT, Mathf::CUBIC);
}

void GameOverManager::Update() 
{
    if (_isBeginProcess)
    {
        const float factor = _backgroundFader.Fade();
        if (_vanishedBackground)
        {
            _vanishedBackground->Alpha = factor;
        }
    }
}

void GameOverManager::ImGuiDrawPropertysEvent() 
{
    if (nullptr == _vanishedAnimation)
    {
        ImGui::Text("Null Animation");
    }
}

void GameOverManager::ProcessGameOver() 
{
    if (false == _isBeginProcess)
    {
        // 오디오 처리
        UmAudio.FadeOut(); // BGM 페이드 아웃 하면 좋을지도?
        UmAudio.Play("-451910");

        _backgroundFader.Reset();
        if (_vanishedBackground)
        {
            _vanishedBackground->gameObject->ActiveSelf = true;
        }
        if (_vanishedAnimation)
        {
            _vanishedAnimation->gameObject->ActiveSelf = true;
            _vanishedAnimation->Setup();
            _vanishedAnimation->StartAnimation();
            _isBeginProcess = true;
            PushInputLayer();

            const float duration = _vanishedAnimation->Duration;
            const float offset   = duration * 0.6f;

            if (CombatUIManager* combatUI = SingletonComponent<CombatUIManager>::GetInstance())
            {
                combatUI->FadeOut(duration);
            }
            if (QTEUIManager* qteUI = SingletonComponent<QTEUIManager>::GetInstance())
            {
                qteUI->FadeOutBattleGuideUI();
            }
            UmTime.Invoke(this, duration - offset, [this]() { TransitionTitleScene(); });
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
                UmSceneManager.LoadScene(UmFileSystem.GetPathFromGuid(SceneGuid::TITLE).string());
                _isBeginProcess = false;
                PopInputLayer();
                UmAudio.FadeOut();
            }
        });
    }
}

void GameOverManager::PressedAButton(const Input::Controller& controller) 
{
}
