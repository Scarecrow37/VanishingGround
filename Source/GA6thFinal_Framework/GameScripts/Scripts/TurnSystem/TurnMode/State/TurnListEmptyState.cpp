#include "pchScripts.h"
#include "TurnListEmptyState.h"
#include <TurnSystem/TurnMode/TurnMode.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

REGISTER_CLASS(FSMStateFactory, TurnListEmptyState)

TurnListEmptyState::TurnListEmptyState() {}

TurnListEmptyState::~TurnListEmptyState() {}

void TurnListEmptyState::OnAwake() {}

void TurnListEmptyState::OnStart() 
{
    TurnModeStateBase::OnStart();

}

void TurnListEmptyState::OnEnter() 
{
    _turnMode->FinishCurrentTurn();

    CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
    if (combatStartPhase)
    {
        for (auto& character : combatStartPhase->GetCharacters())
        {
            int hp = character->HP;
            if (hp <= 0)
            {
                character->Dead();
            }
        }
    }

    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"턴 리스트를 확인합니다.");
}

void TurnListEmptyState::OnExit() {}

void TurnListEmptyState::OnUpdate() {}
