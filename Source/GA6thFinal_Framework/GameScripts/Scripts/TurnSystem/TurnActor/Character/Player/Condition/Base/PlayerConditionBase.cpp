#include "pchScripts.h"
#include "PlayerConditionBase.h"
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <GameCore/FSM/FiniteStateMachine.h>

PlayerConditionBase::PlayerConditionBase() 
{

}

PlayerConditionBase::~PlayerConditionBase() 
{

}

Player& PlayerConditionBase::GetPlayer()
{
    if (nullptr == _player)
    {
        _player = GetFSM().GetComponent<Player>();
    }
    return *_player;
}