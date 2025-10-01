#include "pch.h"
#include "UISFXNavigationComponent.h"

UISFXNavigationComponent::UISFXNavigationComponent()
{
    _focusInAudioID = DEFAULT_FOCUS_IN_SOUND_ID;
}

UISFXNavigationComponent::~UISFXNavigationComponent() = default;

void UISFXNavigationComponent::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);
    if (callType != FocusCallType::INITIAL)
    {
        UmAudio.Play(_focusInAudioID);
    }
}