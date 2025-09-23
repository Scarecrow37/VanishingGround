#include "pchScripts.h"
#include "RestartStageNavi.h"

UMREAL_COMPONENT(RestartStageNavi)

void RestartStageNavi::Submit() 
{
    if (const Scene* scene = UmSceneManager.GetMainScene())
    {
        const std::string path = scene->Path;
        UmSceneManager.LoadScene(path);
    }
}

void RestartStageNavi::OnEnable()
{
    UINavigationComponent::OnEnable();

    Focus();
}
