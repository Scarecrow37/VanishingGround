#include "pchScripts.h"
#include "TempTransition.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(TempTransition)

TempTransition::TempTransition() = default;
TempTransition::~TempTransition() = default;

void TempTransition::Awake()
{
    if (_singletonObject.TrySingleTon(true))
    {
        auto* sceneTrans = GetComponent<SceneTransitionComponent>();
        sceneTrans->Fade("out", nullptr); 
    }

}
