#include "TurnActor.h"
TurnActor::TurnActor() 
    : 
    _currState(STATE::Dead)
{

}
TurnActor::~TurnActor() = default;

void TurnActor::PlayTurn() 
{
    _currState = STATE::Play;
    OnTurnStart();
}

void TurnActor::Revive() 
{
    _currState = STATE::Wait;
    OnRevive();
}

void TurnActor::Awake() 
{
    gameObject->AddTag(TAG);
}
