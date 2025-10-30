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

void ChildsAnimationsController::FadeIn(size_t index) 
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->FadeIn();
    }
}

void ChildsAnimationsController::FadeInWithTag(std::string_view tag)
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->FadeIn();
        }
    }
}

void ChildsAnimationsController::FadeOut(size_t index) 
{
    if (index < _fadeUIs.size())
    {
        _fadeUIs[index]->FadeOut();
    }
}

void ChildsAnimationsController::FadeOutWithTag(std::string_view tag)
{
    if (auto findIter = _fadeUIsWithTag.find(tag); findIter != _fadeUIsWithTag.end())
    {
        for (auto& fadeUI : findIter->second)
        {
            fadeUI->FadeOut();
        }
    }
}

void ChildsAnimationsController::StartAnimation(size_t index) 
{
    if (index < _spriteAnimations.size())
    {
        _spriteAnimations[index]->StartAnimation();
    }
}

void ChildsAnimationsController::StartAnimationWithTag(std::string_view tag)
{
    if (auto findIter = _spriteAnimationsWithTag.find(tag); findIter != _spriteAnimationsWithTag.end())
    {
        for (auto& spriteAnimation : findIter->second)
        {
            spriteAnimation->StartAnimation();
        }
    }
}

void ChildsAnimationsController::StopAnimation(size_t index) 
{
    if (index < _spriteAnimations.size())
    {
        _spriteAnimations[index]->StopAnimation();
    }
}

void ChildsAnimationsController::StopAnimationWithTag(std::string_view tag)
{
    if (auto findIter = _spriteAnimationsWithTag.find(tag); findIter != _spriteAnimationsWithTag.end())
    {
        for (auto& spriteAnimation : findIter->second)
        {
            spriteAnimation->StopAnimation();
        }
    }
}

void ChildsAnimationsController::Awake() 
{
    FindAnimations();
}