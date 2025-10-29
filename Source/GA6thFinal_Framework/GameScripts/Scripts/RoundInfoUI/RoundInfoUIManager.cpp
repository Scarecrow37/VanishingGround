#include "pchScripts.h"
#include "RoundInfoUIManager.h"
#include "UI/Animations/FadeImageElement/FadeImageElement.h"
#include "UI/Animations/FadeTextElement/FadeTextElement.h"

UMREAL_COMPONENT(RoundInfoUIManager)

RoundInfoUIManager::RoundInfoUIManager() = default;
RoundInfoUIManager::~RoundInfoUIManager() = default;

void RoundInfoUIManager::FadeInfoUI(std::string_view text) 
{
    float showUITime = ShowUITime;
    float duration = Duration * 0.5f;
    if (auto imageComponent = _fadeImageElement.lock())
    {
        if (Mathf::Epsilon < duration)
            imageComponent->FadeDuration = duration;

        imageComponent->Enable = true;
        imageComponent->FadeIn();

        UmTime.Invoke(imageComponent.get(), showUITime + duration, [fadeImage = imageComponent.get()]() 
        { 
            fadeImage->FadeOut(); 
        });
    }

    if (auto textComponent = _fadeTextElement.lock())
    {
        if (Mathf::Epsilon < Duration)
            textComponent->FadeDuration = duration;

        textComponent->Text = text.data();
        textComponent->Enable = true;
        textComponent->FadeIn();

        UmTime.Invoke(textComponent.get(), showUITime + duration, [fadeText = textComponent.get()]() 
        { 
            fadeText->FadeOut(); 
        });
    }
}

void RoundInfoUIManager::Awake() 
{
    Base::Awake();
    FindUIElements();
}

void RoundInfoUIManager::FindUIElements()
{
    Transform::ForeachBFS(transform, [this](Transform* curr) 
    {
        GameObject& object = curr->gameObject;
        if (object.CompareTag("Frame"))
        {
            if (FadeImageElement* imageComponent = object.GetComponent<FadeImageElement>())
            {
                imageComponent->Enable = false;
                _fadeImageElement = imageComponent->GetWeakPtrAs<FadeImageElement>();
            }        
        }
        else if (object.CompareTag("Text"))
        {
            if (FadeTextElement* text = object.GetComponent<FadeTextElement>())
            {
                text->Enable = false;
                _fadeTextElement = text->GetWeakPtrAs<FadeTextElement>();
            }           
        }
    });
}