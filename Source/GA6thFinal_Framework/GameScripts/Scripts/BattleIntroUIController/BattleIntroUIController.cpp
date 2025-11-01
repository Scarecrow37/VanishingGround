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
    float totalDuration = 0.f;
    gameObject->SetActive(true);
    if (_animationController.expired())
    {
        FindAnimations();
    }

    if (auto animation = _animationController.lock())
    {
        std::string stageTag = "Stage " + std::to_string(stage);
        std::string roundTag = "Round " + std::to_string(battleCount);

        float fadeInDuration = 0.f;

        animation->BeginWithTag("Frame");
        float frameDuration = animation->FadeInWithTag("Frame");
        fadeInDuration      = std::max(fadeInDuration, frameDuration);

        animation->BeginWithTag("Background");
        float backgrundDuration = animation->FadeInWithTag("Background");
        fadeInDuration          = std::max(fadeInDuration, backgrundDuration);

        animation->BeginWithTag(stageTag);
        float stageDuration = animation->FadeInWithTag(stageTag);
        fadeInDuration      = std::max(fadeInDuration, stageDuration);

        float roundDuration = animation->StartAnimationWithTag(roundTag);
        fadeInDuration      = std::max(fadeInDuration, roundDuration);
        totalDuration = fadeInDuration;

        float delay = IntroTextShowTime;
        totalDuration += delay;
        
        UmTime.Invoke(animation.get(), totalDuration, 
        [animation = animation.get(), stageTag, roundTag]() 
        {
            animation->FadeOutWithTag("Frame");
            animation->FadeOutWithTag("Background");
            animation->FadeOutWithTag(stageTag);
            animation->FadeOutWithTag(roundTag);
        });

        float fadeOutDuration = 0.f;

        frameDuration   = animation->GetDurationWithTag("Frame");
        fadeOutDuration = std::max(fadeOutDuration, frameDuration);

        backgrundDuration = animation->GetDurationWithTag("Background");
        fadeOutDuration   = std::max(fadeOutDuration, backgrundDuration);

        stageDuration   = animation->GetDurationWithTag(stageTag);
        fadeOutDuration = std::max(fadeOutDuration, stageDuration);

        roundDuration   = animation->GetDurationWithTag(roundTag);
        fadeOutDuration = std::max(fadeOutDuration, roundDuration);

        totalDuration += fadeOutDuration;
    }

    UmTime.Invoke(this, totalDuration, [this]() 
    { 
        gameObject->SetActive(false); 
    });
    return totalDuration;
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
        if (ChildsAnimationsController* controller = animationsPanel.GetComponent<ChildsAnimationsController>())
        {
            _animationController = controller->GetWeakPtrAs<ChildsAnimationsController>();
        }
    }
}