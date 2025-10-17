#include "pchScripts.h"
#include "IntroManager.h"
#include "UI/Animations/FadeDescriptionPanel/FadeDescriptionPanel.h"
#include "UI/Animations/FadeTextElement/FadeTextElement.h"

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

    _normalLevelText = GetFadeTextElement("Normal Level Text");
    _hardLevelText   = GetFadeTextElement("Hard Level Text");
    _promptText      = GetFadeTextElement("Intro Prompt");
}

void IntroManager::Update()
{
    Component::Update();

    if (_step == END)
        return;

    if (_step != WAIT_PROMPT || true == _isLevelSelected)
        _elapsedTime += UmTime.DeltaTime();

    switch (_step)
    {
    case WAIT_INTRO_DESCRIPTION:
        if (_elapsedTime >= GetWaitDescriptionTime())
        {
            if (nullptr != _introDescription)
            {
                _introDescription->FadeIn();
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
            if (nullptr != _normalLevelText)
            {
                _normalLevelText->FadeIn();
            }
            if (nullptr != _hardLevelText)
            {
                _hardLevelText->FadeIn();
            }
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
        if (_elapsedTime >= GetWaitPromptTime())
        {
            if (nullptr != _promptText)
            {
                _promptText->FadeIn();
            }
            _step = FADE_IN_PROMPT;
        }
        break;
    case FADE_IN_PROMPT:
        if (_elapsedTime >= GetFadePromptTime())
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
    _normalLevelText  = nullptr;
    _hardLevelText    = nullptr;
    _promptText       = nullptr;
    _elapsedTime      = 0.0f;
}

void IntroManager::ImGuiDrawPropertysEvent()
{
    Component::ImGuiDrawPropertysEvent();

    ImGui::Checkbox("Your mom", &_isLevelSelected);
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

float IntroManager::GetWaitPromptTime() const
{
    return GetFadeLevelSelectionTime() + ReflectFields->PromptDelay;
}

float IntroManager::GetFadePromptTime() const
{
    return GetWaitPromptTime() + ReflectFields->FadeDuration;
}

FadeTextElement* IntroManager::GetFadeTextElement(const std::string& tag) const
{
    const std::weak_ptr<GameObject> introDescriptionObjectWeak = GameObject::FindWithTag(tag);

    if (const std::shared_ptr introDescriptionObject = introDescriptionObjectWeak.lock();
        nullptr != introDescriptionObject)
    {
        FadeTextElement* component = introDescriptionObject->GetComponent<FadeTextElement>();
        component->FadeDuration    = ReflectFields->FadeDuration;
        return component;
    }
    return nullptr;
}