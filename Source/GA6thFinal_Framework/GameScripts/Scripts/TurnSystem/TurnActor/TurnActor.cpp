#include "pchScripts.h"
#include "TurnActor.h"

TurnActor::TurnActor() 
    : 
    _currState(STATE::Dead)
{

}
TurnActor::~TurnActor() = default;

void TurnActor::PlayTurn() 
{
    if (_currState == STATE::Wait)
    {
        _currState = STATE::Play;
    }
}

void TurnActor::Revive() 
{
    if (_currState == STATE::Dead)
    {
        _currState = STATE::Wait;
    }
}

void TurnActor::Dead() 
{
    if (_currState != STATE::Dead)
    {
        _currState = STATE::Dead;
    }
}

void TurnActor::EndTurn() 
{
    if (_currState == STATE::Play)
    {
        _currState = STATE::Wait;
    }
}

void TurnActor::Awake() 
{
    gameObject->AddTag(TAG);
}
