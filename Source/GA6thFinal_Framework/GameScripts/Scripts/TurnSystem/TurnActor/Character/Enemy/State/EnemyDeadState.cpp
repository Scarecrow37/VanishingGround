#include "pchScripts.h"
#include "EnemyDeadState.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyDeadState)

void EnemyDeadState::OnAwake() {}

void EnemyDeadState::OnStart() {}

void EnemyDeadState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"사망.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
    Enemy& enemy = GetEnemy();
    enemy.Dead();
    enemy.SetMainAnimation(CharacterBase::DEATH, false);
}

void EnemyDeadState::OnExit() {}

void EnemyDeadState::OnUpdate() {}
