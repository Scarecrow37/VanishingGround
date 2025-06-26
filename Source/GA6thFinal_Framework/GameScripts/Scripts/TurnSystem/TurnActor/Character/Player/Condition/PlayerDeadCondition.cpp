#include "pchScripts.h"
#include "PlayerDeadCondition.h"
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <GameCore/FSM/Factory/FSMConditionFactory.h>

REGISTER_CLASS(FSMConditionFactory, PlayerDeadCondition)

PlayerDeadCondition::PlayerDeadCondition() 
{}

PlayerDeadCondition::~PlayerDeadCondition() 
{}

void PlayerDeadCondition::OnAwake() 
{}

void PlayerDeadCondition::OnStart() 
{}

bool PlayerDeadCondition::Evaluate()
{
    int hp = GetPlayer().HP;
    bool isDead = 0 >= hp;
    return isDead;
}
