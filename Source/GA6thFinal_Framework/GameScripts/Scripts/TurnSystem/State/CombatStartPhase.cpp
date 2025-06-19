#include "CombatStartPhase.h"

REGISTER_CLASS(FSMStateFactory, CombatStartPhase)

CombatStartPhase::CombatStartPhase()
    : 
    _phaseEnd(false)
{

}

CombatStartPhase::~CombatStartPhase() {}

void CombatStartPhase::OnAwake() {}

void CombatStartPhase::OnStart() {}

void CombatStartPhase::OnEnter() 
{
    UmLogger.Message(LogLevel::LEVEL_TRACE, (const char*)u8"배틀 시작!!!!");
    UmTime.Invoke(&GetFSM(), 3.f, [this]() { this->_phaseEnd = true; });
}

void CombatStartPhase::OnExit() {}

void CombatStartPhase::OnUpdate() {}
