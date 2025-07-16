#include "pchScripts.h"
#include "RoundStartPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <WeaponSystem/WeaponSystem.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>

REGISTER_CLASS(FSMStateFactory, RoundStartPhase)

RoundStartPhase::RoundStartPhase() 
    : 
    _isPhaseEnd(false) 
{

}

RoundStartPhase::~RoundStartPhase() {}

void RoundStartPhase::OnAwake() {}

void RoundStartPhase::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void RoundStartPhase::OnEnter() 
{
    _isPhaseEnd = false;

    _weaponSystem->RollRandomSpeed();
    int currRound = _turnMode->AddRoundCount();

    std::string message = std::format("{}{}", currRound, (const char*)u8"라운드 시작!!!!===========================================================");
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

    _turnMode->MakeTurnList();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"턴 리스트 생성.");

    _turnMode->SortTurnList();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"턴 정렬 완료.");

    NotifyRoundStart();
    _isPhaseEnd = true;
}

void RoundStartPhase::OnExit() 
{
    
}

void RoundStartPhase::OnUpdate() 
{

}
void RoundStartPhase::NotifyRoundStart()
{
    if (_turnMode)
    {
        CombatStartPhase* combatStartPhase = _turnMode->States->CombatStartPhase;
        if (combatStartPhase)
        {
            for (auto& character : combatStartPhase->GetCharacters())
            {
                character->OnRoundStart();
            }
        }
        _turnMode->ApplyActions([](TurnAction& action) { action.OnRoundStart(); });
    }
}