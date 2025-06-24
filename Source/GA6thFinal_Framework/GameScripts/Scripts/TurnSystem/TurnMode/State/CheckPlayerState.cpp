#include "CheckPlayerState.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REGISTER_CLASS(FSMStateFactory, CheckPlayerState)

CheckPlayerState::CheckPlayerState() {}

CheckPlayerState::~CheckPlayerState() {}

void CheckPlayerState::OnAwake() {}

void CheckPlayerState::OnStart() 
{
    TurnModeStateBase::OnStart();
}

void CheckPlayerState::OnEnter() 
{
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"턴을 시작합니다.");
    _turnMode->PopTurnList();
}

void CheckPlayerState::OnExit() {}

void CheckPlayerState::OnUpdate() {}
