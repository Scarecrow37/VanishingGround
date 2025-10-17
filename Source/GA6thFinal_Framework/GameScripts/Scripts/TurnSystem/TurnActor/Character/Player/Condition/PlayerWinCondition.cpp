#include "pchScripts.h"
#include "PlayerWinCondition.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStatsComponent.h"
#include "Stats/Enemy/EnemyStats.h"
#include "GameCore/FSM/FiniteStateMachine.h"

REGISTER_CLASS(FSMConditionFactory, PlayerWinCondition)

PlayerWinCondition::PlayerWinCondition() {}

PlayerWinCondition::~PlayerWinCondition() {}

void PlayerWinCondition::OnAwake() {}

void PlayerWinCondition::OnStart() {}

bool PlayerWinCondition::Evaluate()
{
    TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance();
    if (turnMode)
    {
        auto& enemies = turnMode->GetEnemies();
        for (auto& enemy : enemies)
        {
            EnemyStatsComponent* statsComponent = enemy->GetEnemyStats();
            if (statsComponent)
            {
                EnemyStats& stats = statsComponent->GetStats();
                if (0 < stats.CurrentHP)
                {
                    return false;
                }
            }
        }
        return true;
    }
    return false;
}
