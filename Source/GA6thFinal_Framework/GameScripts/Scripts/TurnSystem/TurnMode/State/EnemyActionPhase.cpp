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
    if (_turnMode)
    {
        if (auto& actorModel = _turnMode->GetCurrTurnActor())
        {
            actorModel.Apply([this](TurnActor* actor) {
                actor->PlayTurn();
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
                actor->UpdatePostTurnState();
            });
        }
        // 액터의 턴 State를 상태 플래그를 확인하여 바꿉니다.
    }

    ApplyReduceHP();
}

void EnemyActionPhase::OnExit() 
{
    ApplyReduceHP();
}

void EnemyActionPhase::OnUpdate() {}
