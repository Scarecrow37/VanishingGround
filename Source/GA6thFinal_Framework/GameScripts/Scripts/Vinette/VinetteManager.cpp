#include "pchScripts.h"
#include "VinetteManager.h"
#include "UI/Panels/Overlay/OverlayPanel.h"
#include "UI/Elements/SpriteAnimation/SpriteAnimationElement.h"

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
                _hitVinette = child->gameObject->GetComponent<SpriteAnimationElement>();
                if (_hitVinette)
                {
                    _hitVinette->StopAnimation();
                }
                child->gameObject->ActiveSelf = false;           
            }
            if (child->gameObject->CompareTag("Heal Vinette"))
            {
                _healVinette = child->gameObject->GetComponent<SpriteAnimationElement>();
                if (_healVinette)
                {
                    _healVinette->StopAnimation();
                }          
                child->gameObject->ActiveSelf = false;
            }
        }
    }
}

void VinetteManager::ShowHitVinette() 
{
    if (_hitVinette)
    {
        _hitVinette->gameObject->ActiveSelf = true;
        _hitVinette->Setup();
        _hitVinette->StartAnimation();
    }
}

void VinetteManager::ShowHealVinette()
{
    if (_healVinette)
    {
        _healVinette->gameObject->ActiveSelf = true;
        _healVinette->Setup();
        _healVinette->StartAnimation();
    }
}
