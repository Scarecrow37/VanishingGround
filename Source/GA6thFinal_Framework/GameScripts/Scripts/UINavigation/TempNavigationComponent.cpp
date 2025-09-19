#include "pchScripts.h"
#include "TempNavigationComponent.h"

TempNavigationComponent::TempNavigationComponent() = default;

void TempNavigationComponent::FocusIn() 
{
    UINavigationComponent::FocusIn();

    UmLogger.Log(LogLevel::LEVEL_INFO, "Focus In");
}

void TempNavigationComponent::FocusOut()
{
    UINavigationComponent::FocusOut();

    UmLogger.Log(LogLevel::LEVEL_INFO, "Focus Out");
}

void TempNavigationComponent::Submit()
{
    UINavigationComponent::Submit();

    UmLogger.Log(LogLevel::LEVEL_INFO, "Submit");
}