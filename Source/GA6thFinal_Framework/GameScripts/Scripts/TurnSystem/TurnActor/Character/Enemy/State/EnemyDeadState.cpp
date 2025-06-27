#include "pchScripts.h"
#include "EnemyDeadState.h"

void EnemyDeadState::OnAwake() {}

void EnemyDeadState::OnStart() {}

void EnemyDeadState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;
    static std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"사망.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

void EnemyDeadState::OnExit() {}

void EnemyDeadState::OnUpdate() {}
