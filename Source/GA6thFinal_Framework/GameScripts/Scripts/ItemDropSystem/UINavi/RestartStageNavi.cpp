#include "pchScripts.h"
#include "RestartStageNavi.h"
#include "SceneTransition/TransitionManager.h"

UMREAL_COMPONENT(RestartStageNavi)

void RestartStageNavi::Submit() 
{
    if (const Scene* scene = UmSceneManager.GetMainScene())
    {
        const std::string path = scene->Path;

        GameObject* transitionmanager = SingletonObject<TransitionManager>::GetInstance();
        transitionmanager->GetComponent<TransitionManager>()->SceneTransitionFade(
            "in", "out", [path]() { UmSceneManager.LoadScene(path); });

    }
}

void RestartStageNavi::OnEnable()
{
    UINavigationComponent::OnEnable();

    Focus();
}
