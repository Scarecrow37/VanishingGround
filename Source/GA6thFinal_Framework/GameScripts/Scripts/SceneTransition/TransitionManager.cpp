#include "pchScripts.h"
#include "TransitionManager.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(TransitionManager)

TransitionManager::TransitionManager() = default;
TransitionManager::~TransitionManager() = default;

void TransitionManager::SceneTransitionFade(std::string_view inPreset, std::string_view outPreset,
                                            std::function<void(void)> callback)
{
    _transitionController->Fade(inPreset, [callback, outPreset, this]() {
        callback();
        _transitionController->Fade(outPreset, nullptr);
    });
}

void TransitionManager::Awake()
{
    if (_singletonObject.TrySingleTon(true))
    {
        _transitionController = GetComponent<SceneTransitionComponent>();
    }
}
