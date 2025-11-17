#include "pchScripts.h"
#include "ChildsAnimationsController.h"
#include "UI/Animations/FadeUIComponent/FadeUIComponent.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

UMREAL_COMPONENT(ChildsAnimationsController)

ChildsAnimationsController::ChildsAnimationsController() = default;
ChildsAnimationsController::~ChildsAnimationsController() = default;

void ChildsAnimationsController::FindAnimations() 
{
    _fadeUIs.clear();
    _fadeUIsWithTag.clear();
    _spriteAnimations.clear();
    _spriteAnimationsWithTag.clear();
    for (int i = 0; i < transform->ChildCount; ++i)
    {
        Transform* child = transform->GetChild(i);
        if (child)
        {
            GameObject& object = child->gameObject;
            const auto& tags = object.GetTags();

            if (FadeUIComponent* fadeUI = object.GetComponent<FadeUIComponent>())
            {
                _fadeUIs.push_back(fadeUI);
                for (auto& tag : tags)
                {
                    _fadeUIsWithTag[tag].push_back(fadeUI);
                }
            }

            if (SpriteAnimationElement* spriteAnimation = object.GetComponent<SpriteAnimationElement>())
            {
                _spriteAnimations.push_back(spriteAnimation);
                for (auto& tag : tags)
                {
                    _spriteAnimationsWithTag[tag].push_back(spriteAnimation);
                }
            }
        }
    }
}

void ChildsAnimationsController::Begin(size_t index) 
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->Begin();
    }
}

void ChildsAnimationsController::BeginWithTag(const std::string& tag) 
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->Begin();
        }
    }
}

float ChildsAnimationsController::FadeIn(size_t index) 
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->FadeIn();
        return _fadeUIs[index]->FadeDuration;
    }
    return 0.f;
}

float ChildsAnimationsController::FadeInWithTag(const std::string& tag)
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        float maxDuration = 0.f;
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->FadeIn();
            maxDuration = std::max(maxDuration, (float)fadeUI->FadeDuration);
        }
        return maxDuration;
    }
    return 0.f;
}

void ChildsAnimationsController::End(size_t index) 
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->End();
    }
}

void ChildsAnimationsController::EndWithTag(const std::string& tag) 
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->End();
        }
    }
}

float ChildsAnimationsController::FadeOut(size_t index)
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->FadeOut();
        return _fadeUIs[index]->FadeDuration;
    }
    return 0.f;
}

float ChildsAnimationsController::FadeOutWithTag(const std::string& tag)
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        float maxDuration = 0.f;
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->FadeOut();
            maxDuration = std::max(maxDuration, (float)fadeUI->FadeDuration);
        }
        return maxDuration;
    }
    return 0.f;
}

float ChildsAnimationsController::GetFadeDurationWithTag(const std::string& tag)
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        float maxDuration = 0.f;
        for (auto& fadeUI : findIter->second)
        {
            maxDuration = std::max(maxDuration, (float)fadeUI->FadeDuration);
        }
        return maxDuration;
    }
    return 0.0f;
}

void ChildsAnimationsController::EnableAnimation(size_t index, bool enable)
{
    if (index < _spriteAnimations.size())
    {
        _spriteAnimations[index]->Enable = enable;
    }
}

float ChildsAnimationsController::StartAnimation(size_t index)
{
    if (index < _spriteAnimations.size())
    {
        _spriteAnimations[index]->Setup();
        _spriteAnimations[index]->StartAnimation();
        return _spriteAnimations[index]->Duration;
    }
    return 0.f;
}

float ChildsAnimationsController::StartAnimationWithTag(const std::string& tag)
{
    if (auto findIter = _spriteAnimationsWithTag.find(tag); findIter != _spriteAnimationsWithTag.end())
    {
        float maxDuration = 0.f;
        for (auto& spriteAnimation : findIter->second)
        {
            maxDuration = std::max(maxDuration, (float)spriteAnimation->Duration);
            spriteAnimation->Setup();
            spriteAnimation->StartAnimation();
        }
        return maxDuration;
    }
    return 0.f;
}

void ChildsAnimationsController::StopAnimation(size_t index)
{
    if (index < _spriteAnimations.size())
    {
        _spriteAnimations[index]->StopAnimation();
    }
}

void ChildsAnimationsController::StopAnimationWithTag(const std::string& tag)
{
    if (auto findIter = _spriteAnimationsWithTag.find(tag); findIter != _spriteAnimationsWithTag.end())
    {
        for (auto& spriteAnimation : findIter->second)
        {
            spriteAnimation->StopAnimation();
        }
    }
}

float ChildsAnimationsController::GetAnimationDurationWithTag(const std::string& tag)
{
    if (auto findIter = _spriteAnimationsWithTag.find(tag); findIter != _spriteAnimationsWithTag.end())
    {
        float maxDuration = 0.f;
        for (auto& spriteAnimation : findIter->second)
        {
            maxDuration = std::max(maxDuration, (float)spriteAnimation->Duration);
        }
        return maxDuration;
    }
    return 0.f;
}

void ChildsAnimationsController::Added() 
{
    if (UmCore->IsPlay())
    {
        FindAnimations();
    } 
}

bool ChildsAnimationsController::IsFadeComplete(size_t index) const
{
    if (index < _fadeUIs.size())
    {
        return _fadeUIs[index]->IsComplete();
    }
    return true;
}

void ChildsAnimationsController::CompleteFadeImmediately(size_t index)
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->CompleteImmediately();
    }
}