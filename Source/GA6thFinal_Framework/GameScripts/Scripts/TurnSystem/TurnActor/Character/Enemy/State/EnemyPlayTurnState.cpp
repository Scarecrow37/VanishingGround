#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

void EnemyPlayTurnState::OnAwake() 
{

}

void EnemyPlayTurnState::OnStart() 
{

}

void EnemyPlayTurnState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 3.");
    UmTime.Invoke(&GetFSM(), 1.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 2.");
    UmTime.Invoke(&GetFSM(), 2.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료 1.");
    UmTime.Invoke(&GetFSM(), 3.f, [=]() { UmLogger.Message(LogLevel::LEVEL_TRACE, message); });

    UmTime.Invoke(&GetFSM(), 4.f, [=]() { GetEnemy().EndTurn(); });
}

void EnemyPlayTurnState::OnExit() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}

void EnemyPlayTurnState::OnUpdate()
{

}
