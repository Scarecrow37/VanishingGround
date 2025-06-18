#include "BattleStartPhase.h"

REGISTER_CLASS(FSMStateFactory, BattleStartPhase)

BattleStartPhase::BattleStartPhase() {}

BattleStartPhase::~BattleStartPhase() {}

void BattleStartPhase::OnAwake() {}

void BattleStartPhase::OnStart() {}

void BattleStartPhase::OnEnter() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작!!!!");
}

void BattleStartPhase::OnExit() {}

void BattleStartPhase::OnUpdate() {}
