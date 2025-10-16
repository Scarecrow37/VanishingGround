#include "pchScripts.h"
#include "IntroManager.h"
#include "UI/Animations/FadeDescriptionPanel/FadeDescriptionPanel.h"

UMREAL_COMPONENT(IntroManager)

IntroManager::IntroManager() = default;

void IntroManager::Start()
{
    Component::Start();

    const std::weak_ptr<GameObject> introDescriptionObjectWeak = GameObject::FindWithTag("Intro Description");

    if (const std::shared_ptr introDescriptionObject = introDescriptionObjectWeak.lock(); nullptr != introDescriptionObject)
    {
        _introDescription = introDescriptionObject->GetComponent<FadeDescriptionPanel>();
        _introDescription->FadeDuration = ReflectFields->FadeDuration;
    }
}

void IntroManager::Update()
{
    Component::Update();

    if (_step == END)
        return;

    _elapsedTime += UmTime.DeltaTime();

    switch (_step)
    {
    case WAIT_INTRO_DESCRIPTION:
        if (_elapsedTime >= GetWaitDescriptionTime())
        {
            if (nullptr != _introDescription)
            {
                _introDescription->StartFade();
            }
            _step = FADE_IN_INTRO_DESCRIPTION;
        }
        break;
    case FADE_IN_INTRO_DESCRIPTION:
        if (_elapsedTime >= GetFadeDescriptionTime())
        {
            _step = WAIT_LEVEL_SELECTION;
        }
        break;
    case WAIT_LEVEL_SELECTION:
        if (_elapsedTime >= GetWaitLevelSelectionTime())
        {
            _step = FADE_IN_LEVEL_SELECTION;
        }
        break;
    case FADE_IN_LEVEL_SELECTION:
        if (_elapsedTime >= GetFadeLevelSelectionTime())
        {
            _step = WAIT_PROMPT;
        }
        break;
    case WAIT_PROMPT:
        if (_elapsedTime >= GetWaitLevelSelectionTime() + ReflectFields->PromptDelay)
        {
            _step = END;
        }
        break;
    case FADE_IN_PROMPT:
        if (_elapsedTime >= GetFadeLevelSelectionTime() + ReflectFields->PromptDelay + ReflectFields->FadeDuration)
        {
            _step = END;
        }
        break;
    case END:
        break;
    }
}

void IntroManager::Reset()
{
    Component::Reset();

    _step             = WAIT_INTRO_DESCRIPTION;
    _introDescription = nullptr;
    _elapsedTime      = 0.0f;
}

float IntroManager::GetWaitDescriptionTime() const
{
    return ReflectFields->DescriptionDelay;
}

float IntroManager::GetFadeDescriptionTime() const
{
    return GetWaitDescriptionTime() + ReflectFields->FadeDuration;
}

float IntroManager::GetWaitLevelSelectionTime() const
{
    return GetFadeDescriptionTime() + ReflectFields->LevelSelectDelay;
}

float IntroManager::GetFadeLevelSelectionTime() const
{
    return GetWaitLevelSelectionTime() + ReflectFields->FadeDuration;
}