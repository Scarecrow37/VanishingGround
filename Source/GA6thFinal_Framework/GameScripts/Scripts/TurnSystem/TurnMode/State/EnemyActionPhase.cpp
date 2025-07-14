#include "pchScripts.h" 
#include "EnemyActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyActionPhase)

EnemyActionPhase::EnemyActionPhase() {}

EnemyActionPhase::~EnemyActionPhase() {}

void EnemyActionPhase::OnAwake() {}

void EnemyActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void EnemyActionPhase::OnEnter() 
{
    TurnActor* actor = _turnMode->GetCurrTurnActor();
    actor->PlayTurn();

    if (_turnMode)
    {
        actor->OnTurnStart();

        auto* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            Player* player = combatStartPhase->GetPlayer();
            if (player)
            {
                player->OnEachTurnStart(static_cast<CharacterBase*>(actor));
            }
            const auto& enemies = combatStartPhase->GetEnemies();
            for (const auto& enemy : enemies)
            {
                if (enemy)
                {
                    enemy->OnEachTurnStart(static_cast<CharacterBase*>(actor));
                }
            }
        }
    }
}

void EnemyActionPhase::OnExit() 
{
}

void EnemyActionPhase::OnUpdate() {}
