#include "TurnActor.h"
TurnActor::TurnActor() = default;
TurnActor::~TurnActor() = default;

void TurnActor::Awake() 
{
    gameObject->AddTag(TAG);
}
