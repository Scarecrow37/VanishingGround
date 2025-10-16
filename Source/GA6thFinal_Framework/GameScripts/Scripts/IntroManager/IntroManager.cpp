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
    }
}

void IntroManager::Update()
{
    Component::Update();

    _elapsedTime += UmTime.DeltaTime();

    switch (_step)
    {
    case WAIT_INTRO_DESCRIPTION:
        if (_elapsedTime >= ReflectFields->IntroDescriptionDelay)
        {
            if (nullptr != _introDescription)
            {
                _introDescription->StartFade();
            }
            _step = FADE_IN_INTRO_DESCRIPTION;
        }
        break;
    case FADE_IN_INTRO_DESCRIPTION:
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