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
        CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            CharacterBase* character = static_cast<CharacterBase*>(actor);
            for (auto& ch : combatStartPhase->GetCharacters())
            {
                ch->OnEachTurnStart(character);
            }
            _turnMode->ApplyActions([character](TurnAction& action) { action.OnTurnStart(*character); });
        }
    }
}

void EnemyActionPhase::OnExit() 
{
}

void EnemyActionPhase::OnUpdate() {}
