#include "pchScripts.h"
#include "TurnListEmptyState.h"

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
    UmLogger.Message(LogLevel::LEVEL_DEBUG, (const char*)u8"턴 리스트를 확인합니다.");
}

void TurnListEmptyState::OnExit() {}

void TurnListEmptyState::OnUpdate() {}
