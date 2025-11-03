#include "pchScripts.h"
#include "GameOverCondition.h"

#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>

REGISTER_CLASS(FSMConditionFactory, GameOverCondition)

GameOverCondition::GameOverCondition() 
{

}

GameOverCondition::~GameOverCondition() 
{

}

void GameOverCondition::OnAwake() 
{

}

bool GameOverCondition::Evaluate()
{
    CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
    if (combatStartPhase)
    {
        Player* player = combatStartPhase->GetPlayer();
        if (player)
        {
            return player->gameObject->ActiveSelf == false;
        }
    }
    return false;
}
