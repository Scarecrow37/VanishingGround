#include "pch.h"
#include "UISFXNavigationComponent.h"

UISFXNavigationComponent::UISFXNavigationComponent()
{
    _focusInAudioID = DEFAULT_FOCUSE_IN_SOUND_ID;
}

UISFXNavigationComponent::~UISFXNavigationComponent() = default;

void UISFXNavigationComponent::FocusIn()
{
    UmAudio.Play(_focusInAudioID);
}