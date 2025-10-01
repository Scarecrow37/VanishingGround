#include "pchScripts.h"
#include "RestartStageNavi.h"
#include "SceneTransition/TransitionManager.h"

UMREAL_COMPONENT(RestartStageNavi)

void RestartStageNavi::Submit() 
{
    if (const Scene* scene = UmSceneManager.GetMainScene())
    {
        const std::string& path = scene->Path;
        GameObject* transitionManager = SingletonObject<TransitionManager>::GetInstance();
        if (transitionManager)
        {
            auto transitionComponent = transitionManager->GetComponent<TransitionManager>();
            if (transitionComponent)
            {
                transitionComponent->SceneTransitionFade("in", "out", [path]() { UmSceneManager.LoadScene(path); });
            }
        }
    }
}

void RestartStageNavi::OnEnable()
{
    UINavigationComponent::OnEnable();

    Focus();
}
