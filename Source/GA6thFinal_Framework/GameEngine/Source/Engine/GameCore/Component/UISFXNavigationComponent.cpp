#include "pch.h"
#include "UISFXNavigationComponent.h"

REFLECT_FUNCTION(UISFXNavigationComponent)

UISFXNavigationComponent::UISFXNavigationComponent()
{
    _focusInAudioID = DEFAULT_FOCUS_IN_SOUND_ID;
    _submitAudioID  = DEFAULT_SUBMIT_SOUND_ID;
}

UISFXNavigationComponent::~UISFXNavigationComponent() = default;

void UISFXNavigationComponent::FocusIn(const FocusCallType callType)
{
    Base::FocusIn(callType);
    if (callType != FocusCallType::INITIAL)
    {
        if (EnableInHierarchy)
        {
            UmAudio.Play(_focusInAudioID);
        }
    }
}

void UISFXNavigationComponent::Submit() 
{
    Base::Submit();
    if (EnableInHierarchy)
    {
        UmAudio.Play(_submitAudioID);
    }
}

void UISFXNavigationComponent::SetFocusInAudioID(const std::string& audioID) 
{
    _focusInAudioID = audioID;
}

void UISFXNavigationComponent::SetSubmitAudioID(const std::string& audioID) 
{
    _submitAudioID = audioID;
}
