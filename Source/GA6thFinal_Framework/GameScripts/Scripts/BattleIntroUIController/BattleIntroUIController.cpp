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
    if ( _roundsController.expired() || _stagesController.expired() || _framesController.expired())
    {   
        FindAnimations();
    }
    gameObject->SetActive(true);

    auto framesController = _framesController.lock();
    auto roundsController = _roundsController.lock();
    auto stagesController = _stagesController.lock();
    
    std::string stageTag = "Stage " + std::to_string(stage);

    //페이드 인 연출
    {
        float fadeInDuration = 0.f;
        if (framesController)
        {
            fadeInDuration = framesController->FadeDuration;
            framesController->Begin();
            framesController->FadeIn();
        }
        if (roundsController)
        {
            roundsController->BeginWithTag("Round 1");
            float duration = roundsController->FadeInWithTag("Round 1");
            fadeInDuration = std::max(fadeInDuration, duration);

            roundsController->BeginWithTag("Round 2");
            duration       = roundsController->FadeInWithTag("Round 2");
            fadeInDuration = std::max(fadeInDuration, duration);

            roundsController->BeginWithTag("Round 3");
            duration       = roundsController->FadeInWithTag("Round 3");
            fadeInDuration = std::max(fadeInDuration, duration);
        }
        if (stagesController)
        {
            stagesController->BeginWithTag(stageTag);
            float duration = stagesController->FadeInWithTag(stageTag);
            fadeInDuration = std::max(fadeInDuration, duration);
        }
        totalDuration += fadeInDuration;
    }
       
    //텍스트 연출
    {
        UmTime.Invoke(this, totalDuration, [this, battleCount]() 
        {
            if (auto roundsController = _roundsController.lock())
            {
                std::string roundTag;
                for (int i = 1; i <= battleCount; ++i)
                {
                    roundTag = "Round " + std::to_string(i);
                    roundsController->StopAnimationWithTag(roundTag);
                    roundsController->StartAnimationWithTag(roundTag);
                }
            }
        });
        float animationDuration = IntroTextShowTime;
        if (roundsController)
        {
            std::string roundTag;
            for (int i = 1; i <= battleCount; ++i)
            {
                roundTag = "Round " + std::to_string(battleCount);
                animationDuration = std::max(animationDuration, roundsController->GetAnimationDurationWithTag(roundTag));
            }        
        }
        totalDuration += animationDuration;
    }
  
    //페이드 아웃 연출
    {
        float fadeOutDuration = 0.f;
        if (framesController)
        {
            fadeOutDuration = framesController->FadeDuration;
        }
        UmTime.Invoke(this, totalDuration, [this]() 
        {
            // 사라질때 소리 재생
            UmAudio.Play("-411000");

            if (auto framesController = _framesController.lock())
            {
                framesController->FadeOut();
            }
        });

        if (roundsController)
        {
            float duration  = roundsController->GetFadeDurationWithTag("Round 1");
            fadeOutDuration = std::max(fadeOutDuration, duration);

            duration        = roundsController->GetFadeDurationWithTag("Round 2");
            fadeOutDuration = std::max(fadeOutDuration, duration);

            duration        = roundsController->GetFadeDurationWithTag("Round 3");
            fadeOutDuration = std::max(fadeOutDuration, duration);
        }
        UmTime.Invoke(this, totalDuration, [this]() 
        {
            if (auto roundsController = _roundsController.lock())
            {
                roundsController->FadeOutWithTag("Round 1");
                roundsController->FadeOutWithTag("Round 2");
                roundsController->FadeOutWithTag("Round 3");
            }
        });

        if (stagesController)
        {    
            float duration = stagesController->GetFadeDurationWithTag(stageTag);
            fadeOutDuration = std::max(fadeOutDuration, duration);
        }
        UmTime.Invoke(this, totalDuration, [this, stageTag]() 
        {
            if (auto stagesController = _stagesController.lock())
            {
                stagesController->FadeOutWithTag(stageTag);
            }
        });

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
    for (int i = 0; i < transform->ChildCount; ++i)
    {
        if (Transform* child = transform->GetChild(i))
        {
            GameObject& object = child->gameObject;
            if (object.CompareTag("Frames Panel"))
            {
                if (FadeUIComponent* com = object.GetComponent<FadeUIComponent>())
                {
                    _framesController = com->GetWeakPtrAs<FadeUIComponent>();
                }
            }
            else if (object.CompareTag("Rounds Panel"))
            {
                if (ChildsAnimationsController* com = object.GetComponent<ChildsAnimationsController>())
                {
                    _roundsController = com->GetWeakPtrAs<ChildsAnimationsController>();
                }
            }
            else if (object.CompareTag("Stages Panel"))
            {
                if (ChildsAnimationsController* com = object.GetComponent<ChildsAnimationsController>())
                {
                    _stagesController = com->GetWeakPtrAs<ChildsAnimationsController>();
                }
            }
        }   
    }
}