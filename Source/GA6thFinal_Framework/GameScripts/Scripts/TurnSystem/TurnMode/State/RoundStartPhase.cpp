#include "pchScripts.h"
#include "RoundStartPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"
#include <WeaponSystem/WeaponSystem.h>
#include <TurnSystem/TurnMode/State/CombatStartPhase.h>
#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <RevelationSystem/RevelationSystem.h>

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
    /// 사운드
    UmAudio.Play("-20100");

    _isPhaseEnd = false;

    if (_weaponSystem)
    {
        _weaponSystem->RollRandomSpeed();
    }
    int currRound = _turnMode->AddRoundCount();

    std::string message = std::format("{}{}", currRound, (const char*)u8"라운드 시작!!!!===========================================================");
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);

    _turnMode->MakeTurnList();
    _turnMode->SortTurnList();

    if (_revelationSystem)
    {
        _revelationSystem->RollRoundElement();
    }

    NotifyRoundStart();

    //캐릭터 사망 확인
    UpdateCharacterDead();   

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