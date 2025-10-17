#include "pchScripts.h"
#include "IntroManager.h"
#include "UI/Animations/FadeDescriptionPanel/FadeDescriptionPanel.h"
#include "UI/Animations/FadeImageElement/FadeImageElement.h"
#include "UI/Animations/FadeTextElement/FadeTextElement.h"

UMREAL_COMPONENT(IntroManager)

IntroManager::IntroManager() = default;

void IntroManager::Awake()
{
    Component::Awake();

    BindInputAction(ControllerButton::A, Action::PRESSED, this, &IntroManager::SkipStep);
    BindInputAction(ControllerButton::DPAD_UP, Action::PRESSED, this, &IntroManager::SelectNormal);
    BindInputAction(ControllerButton::DPAD_DOWN, Action::PRESSED, this, &IntroManager::SelectHard);
}

void IntroManager::Start()
{
    Component::Start();

    _introDescription = GetElement<FadeDescriptionPanel>("Intro Description");
    _introDescription->FadeDuration = ReflectFields->FadeDuration;

    _normalLevelText = GetElement<FadeTextElement>("Normal Level Text");
    _normalLevelText->FadeDuration = ReflectFields->FadeDuration;
    _hardLevelText   = GetElement<FadeTextElement>("Hard Level Text");
    _hardLevelText->FadeDuration   = ReflectFields->FadeDuration;
    _promptText      = GetElement<FadeTextElement>("Intro Prompt");
    _promptText->FadeDuration      = ReflectFields->FadeDuration;

    _normalSelection = GetElement<FadeImageElement>("Normal Level Selection");
    _hardSelection   = GetElement<FadeImageElement>("Hard Level Selection");
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
            SelectNormal();
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
    _isLevelSelected  = false;
    _isSelectHard     = false;
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

void IntroManager::SkipStep(const Input::Controller& controller)
{
    switch (_step)
    {
    case WAIT_INTRO_DESCRIPTION:
        _elapsedTime = GetWaitDescriptionTime();
        if (nullptr != _introDescription)
        {
            _introDescription->FadeIn();
        }
        _step = FADE_IN_INTRO_DESCRIPTION;
        break;
    case FADE_IN_INTRO_DESCRIPTION:
        _elapsedTime = GetFadeDescriptionTime();
        if (nullptr != _introDescription)
        {
            _introDescription->End();
        }
        _step = WAIT_LEVEL_SELECTION;
        break;
    case WAIT_LEVEL_SELECTION:
        _elapsedTime = GetWaitLevelSelectionTime();
        if (nullptr != _normalLevelText)
        {
            _normalLevelText->FadeIn();
        }
        if (nullptr != _hardLevelText)
        {
            _hardLevelText->FadeIn();
        }
        _step = FADE_IN_LEVEL_SELECTION;
        break;
    case FADE_IN_LEVEL_SELECTION:
        _elapsedTime = GetFadeLevelSelectionTime();
        if (nullptr != _normalLevelText)
        {
            _normalLevelText->End();
        }
        if (nullptr != _hardLevelText)
        {
            _hardLevelText->End();
        }
        _step = WAIT_PROMPT;
        break;
    case WAIT_PROMPT:
        _isLevelSelected = true;
        break;
    case FADE_IN_PROMPT:
        _step = END;
        break;
    default:
        break;
    }
}

void IntroManager::SelectNormal(const Input::Controller& controller)
{
    SelectNormal();
}

void IntroManager::SelectNormal()
{
    if (_step == WAIT_PROMPT)
    {
        if (_hardSelection)
            _hardSelection->FadeOut();
        if (_normalSelection)
            _normalSelection->FadeIn();
        _isSelectHard = false;
    }
}

void IntroManager::SelectHard(const Input::Controller& controller)
{
    if (_step == WAIT_PROMPT)
    {
        if (_normalSelection)
            _normalSelection->FadeOut();
        if (_hardSelection)
            _hardSelection->FadeIn();
        _isSelectHard = true;
    }
}