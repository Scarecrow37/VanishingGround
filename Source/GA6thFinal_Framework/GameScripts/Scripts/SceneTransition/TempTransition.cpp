#include "pchScripts.h"
#include "TempTransition.h"
#include "SceneTransition/SceneTransitionComponent.h"

UMREAL_COMPONENT(TempTransition)

TempTransition::TempTransition() = default;
TempTransition::~TempTransition() = default;

void TempTransition::Awake()
{
    auto* sceneTrans = GetComponent<SceneTransitionComponent>();
    sceneTrans->Fade("out", nullptr);
}
