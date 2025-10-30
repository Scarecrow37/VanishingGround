#include "pchScripts.h"
#include "GameClearCondition.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>

REGISTER_CLASS(FSMConditionFactory, GameClearCondition)

GameClearCondition::GameClearCondition() 
{
}

GameClearCondition::~GameClearCondition() 
{
}

void GameClearCondition::OnAwake() 
{

}

bool GameClearCondition::Evaluate()
{
    CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
    if (combatStartPhase)
    {
        const std::vector<Enemy*>& enemies = combatStartPhase->GetEnemies();
        if (false == enemies.empty())
        {
            bool result = true;
            for (auto& enemy : enemies)
            {
                result &= enemy->gameObject->ActiveSelf == false;
            }
            return result;
        }
    }
    return false;
}
