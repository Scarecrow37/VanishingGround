#include "pchScripts.h"
#include "RestartStageNavi.h"

UMREAL_COMPONENT(RestartStageNavi)

RestartStageNavi::RestartStageNavi() = default;

RestartStageNavi::~RestartStageNavi() = default;

void RestartStageNavi::FocusIn()
{

}

void RestartStageNavi::FocusOut() 
{

}

void RestartStageNavi::Submit() 
{
    Scene* scene = UmSceneManager.GetMainScene();
    if (scene)
    {
        std::string path = scene->Path;
        UmSceneManager.LoadScene(path);
    }
}
