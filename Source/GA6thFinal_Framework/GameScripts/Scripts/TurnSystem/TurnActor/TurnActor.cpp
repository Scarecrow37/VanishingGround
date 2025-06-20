#include "TurnActor.h"
TurnActor::TurnActor() 
    : 
    _currState(STATE::Dead)
{

}
TurnActor::~TurnActor() = default;

void TurnActor::Awake() 
{
    gameObject->AddTag(TAG);
}
