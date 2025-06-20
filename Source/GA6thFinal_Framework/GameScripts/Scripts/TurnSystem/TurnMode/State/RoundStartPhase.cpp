#include "RoundStartPhase.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_CLASS(FSMStateFactory, RoundStartPhase)

RoundStartPhase::RoundStartPhase() 
{

}

RoundStartPhase::~RoundStartPhase() {}

void RoundStartPhase::OnAwake() {}

void RoundStartPhase::OnStart() 
{
    GetTurnModeBaseS::OnStart();
}

void RoundStartPhase::OnEnter() 
{
    int currRound = _turnMode->AddRoundCount();
    std::string message = std::format("{}{}", currRound, (const char*)u8"라운드 시작!!!!");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    _turnMode->MakeTurnList();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"턴 리스트 생성.");

    _turnMode->SortTurnList();
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"턴 정렬 완료.");

    _turnMode->PopTurnList();   
}

void RoundStartPhase::OnExit() 
{
    
}

void RoundStartPhase::OnUpdate() 
{

}
