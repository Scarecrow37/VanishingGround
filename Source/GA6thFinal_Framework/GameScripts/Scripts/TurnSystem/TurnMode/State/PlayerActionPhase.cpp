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
    if (_turnMode)
    {
        if (auto& actor = _turnMode->GetCurrTurnActor())
        {
            actor->PlayTurn();
            if (auto* combatStartPhase = _turnMode->States->CombatStartPhase)
            {
                CharacterBase* character = static_cast<CharacterBase*>(actor.Get());
                for (auto& cha : combatStartPhase->GetCharacters())
                {
                    cha->OnEachTurnStart(character);
                }
                _turnMode->ApplyActions([character](TurnAction& action) { action.OnTurnStart(*character); });
            }
            actor->UpdatePostTurnState();
        }
    }
}

void PlayerActionPhase::OnExit()
{

}

void PlayerActionPhase::OnUpdate() {}
