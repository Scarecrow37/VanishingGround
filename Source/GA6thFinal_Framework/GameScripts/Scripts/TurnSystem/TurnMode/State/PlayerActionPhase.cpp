#include "pchScripts.h"
#include "PlayerActionPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include "TurnSystem/TurnActor/TurnActor.h"

#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, PlayerActionPhase)

PlayerActionPhase::PlayerActionPhase() 
{
}

PlayerActionPhase::~PlayerActionPhase() 
{
}

void PlayerActionPhase::OnAwake() 
{
}

void PlayerActionPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void PlayerActionPhase::OnEnter() 
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

void PlayerActionPhase::OnExit() {}

void PlayerActionPhase::OnUpdate() {}
