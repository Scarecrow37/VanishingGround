#include "pchScripts.h"
#include "TempNavigationComponent.h"

//UMREAL_COMPONENT(UINavigationComponent);

TempNavigationComponent::TempNavigationComponent() = default;

void TempNavigationComponent::FocusIn(const FocusCallType callType) 
{
    Base::FocusIn(callType);

    UmLogger.Log(LogLevel::LEVEL_INFO, "Focus In");
}

void TempNavigationComponent::FocusOut(const FocusCallType callType)
{
    UINavigationComponent::FocusOut(callType);

    UmLogger.Log(LogLevel::LEVEL_INFO, "Focus Out");
}

void TempNavigationComponent::Submit()
{
    UINavigationComponent::Submit();

    UmLogger.Log(LogLevel::LEVEL_INFO, "Submit");
}