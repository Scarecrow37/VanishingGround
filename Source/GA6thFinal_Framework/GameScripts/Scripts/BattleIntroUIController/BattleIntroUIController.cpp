#include "pchScripts.h"
#include "BattleIntroUIController.h"
#include "UI/Animations/ChildsAnimationsController/ChildsAnimationsController.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"

UMREAL_COMPONENT(BattleIntroUIController)

BattleIntroUIController::BattleIntroUIController() 
    :
    _singletonComponent(this) 
{

}
BattleIntroUIController::~BattleIntroUIController() = default;

float BattleIntroUIController::PlayIntro(int stage, int battleCount)
{
    float introDuration = 0.f;
    gameObject->SetActive(true);
    if (_fadeUIComponent.expired() || _animationController.expired())
    {
        FindAnimations();
    }

    if (auto animation = _animationController.lock())
    {
        std::string stageTag      = "Stage " + std::to_string(stage);
        float       stageDuration = animation->FadeInWithTag(stageTag);
        introDuration             = std::max(introDuration, stageDuration);

        std::string roundTag      = "Round " + std::to_string(battleCount);
        float       roundDuration = animation->StartAnimationWithTag(roundTag);
        introDuration             = std::max(introDuration, roundDuration);

        float delay = IntroTextShowTime;
        introDuration += delay;
    }

    if (auto fadeUI = _fadeUIComponent.lock())
    {
        UmTime.Invoke(fadeUI.get(), introDuration, [fadeUI = fadeUI.get()]()
        { 
            fadeUI->FadeOut();
        });
        introDuration += fadeUI->FadeDuration;
    }
    UmTime.Invoke(this, introDuration, [this]() 
    { 
        gameObject->SetActive(false); 
    });
    return introDuration;
}

void BattleIntroUIController::Added() 
{
    if (UmCore->IsPlay())
    {
        _singletonComponent.TrySingleTon();
        gameObject->SetActive(false);
    }
}

void BattleIntroUIController::FindAnimations()
{
    if (Transform* tr =  transform->FindWithTag("Animations"))
    {
        GameObject& animationsPanel = tr->gameObject; 
        if (FadeUIComponent* fadeUI = animationsPanel.GetComponent<FadeUIComponent>())
        {
            _fadeUIComponent = fadeUI->GetWeakPtrAs<FadeUIComponent>();
        }
        if (ChildsAnimationsController* controller = animationsPanel.GetComponent<ChildsAnimationsController>())
        {
            _animationController = controller->GetWeakPtrAs<ChildsAnimationsController>();
        }
    }
}