#include "pchScripts.h"
#include "TransitionManager.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(TransitionManager)

TransitionManager::TransitionManager() = default;
TransitionManager::~TransitionManager() = default;

void TransitionManager::SceneTransitionFade(std::string_view inPreset, std::string_view outPreset,
                                            std::function<void(void)> callback)
{
    if (true == _isTransitioning)
    {
        return;
    }

    if (nullptr != _transitionController && !_transitionController->IsTransitioning())
    {
        _isTransitioning = true;
        _transitionController->Fade(inPreset, [callback, outPreset, this]() {
            if (nullptr != callback)
            {
                callback();
            }
            _transitionController->Fade(outPreset, [this]() 
                { 
                    _isTransitioning = false; 
                });
        });
    }
}

void TransitionManager::Awake()
{
    if (_singletonObject.TrySingleTon(true))
    {
        _transitionController = GetComponent<SceneTransitionComponent>();
    }
}