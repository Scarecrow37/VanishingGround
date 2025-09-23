#include "pchScripts.h"
#include "RestartStageNavi.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(RestartStageNavi)

void RestartStageNavi::Submit() 
{
    if (const Scene* scene = UmSceneManager.GetMainScene())
    {
        const std::string path = scene->Path;
        auto*             sceneTrans = GetComponent<SceneTransitionComponent>();
        sceneTrans->Fade("in", [&]() { UmSceneManager.LoadScene(path); });
    }
}

void RestartStageNavi::OnEnable()
{
    UINavigationComponent::OnEnable();

    auto* sceneTrans = GetComponent<SceneTransitionComponent>();
    sceneTrans->Fade("out", nullptr);

    Focus();
}
