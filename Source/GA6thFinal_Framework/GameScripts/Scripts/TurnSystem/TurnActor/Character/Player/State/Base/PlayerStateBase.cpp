#include "pchScripts.h"
#include "PlayerStateBase.h"
#include <GameCore/FSM/FiniteStateMachine.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

PlayerStateBase::PlayerStateBase() 
    : 
    _player(nullptr) 
{

}

PlayerStateBase::~PlayerStateBase() 
{

}

Player& PlayerStateBase::GetPlayer()
{
    if (nullptr == _player)
    {
        _player = GetFSM().GetComponent<Player>();
    }
    return *_player;
}


