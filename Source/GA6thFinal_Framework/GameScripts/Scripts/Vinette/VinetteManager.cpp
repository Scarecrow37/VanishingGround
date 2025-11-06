#include "pchScripts.h"
#include "VinetteManager.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Animations/FadeImageElement/FadeImageElement.h"

UMREAL_COMPONENT(VinetteManager)

void VinetteManager::Awake() 
{
    _singletonComponent.TrySingleTon();

    _vinetteOverlay         = gameObject->GetComponent<OverlayPanel>();
    const int childCount    = gameObject->transform->GetChildCount();
    for (int i = 0; i < childCount; ++i)
    {
        if (Transform* child = gameObject->transform->GetChild(i))
        {
            if (child->gameObject->CompareTag("Hit Vinette"))
            {
                _hitVinette = child->gameObject->GetComponent<FadeImageElement>();
            }
            if (child->gameObject->CompareTag("Heal Vinette"))
            {
                _healVinette = child->gameObject->GetComponent<FadeImageElement>();
            }
        }
    }
}

void VinetteManager::ShowHitVinette() 
{
    if (_hitVinette)
    {
        _hitVinette->FadeDuration = Duration;
        _hitVinette->FadeIn();
        UmTime.Invoke(this, Duration, [this]() { _hitVinette->FadeOut(); });
    }
}

void VinetteManager::ShowHealVinette()
{
    if (_healVinette)
    {
        _healVinette->FadeDuration = Duration;
        _healVinette->FadeIn();
        UmTime.Invoke(this, Duration, [this]() { _healVinette->FadeOut(); });
    }
}
