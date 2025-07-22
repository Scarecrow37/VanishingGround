#include "pchScripts.h"
#include "EnemyDeadState.h"
#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include <Mesh/SkeletalMeshRenderer.h>

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
    SkeletalMeshRenderer* renderer = enemy.GetSkeletalMeshRenderer();
    if (renderer)
    {
        renderer->BeginBuildOverrideAnimation();
        renderer->ClearOverrideAnimations();
        enemy.SetMainAnimation(CharacterBase::DEATH, false);
        renderer->EndBuildOverrideAnimation();
    }
}

void EnemyDeadState::OnExit() {}

void EnemyDeadState::OnUpdate() {}
