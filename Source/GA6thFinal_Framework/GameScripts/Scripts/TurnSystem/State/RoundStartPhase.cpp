#include "RoundStartPhase.h"

REGISTER_CLASS(FSMStateFactory, RoundStartPhase)

RoundStartPhase::RoundStartPhase() {}

RoundStartPhase::~RoundStartPhase() {}

void RoundStartPhase::OnAwake() {}

void RoundStartPhase::OnStart() {}

void RoundStartPhase::OnEnter() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"라운드 시작!!!!");
}

void RoundStartPhase::OnExit() {}

void RoundStartPhase::OnUpdate() {}
