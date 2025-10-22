#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

REFLECT_FUNCTION(EnemyPlayTurnState)

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define BUILD_AI(index)             \
case index:                         \
BuildAIModel##index();              \
break;

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
}

void EnemyPlayTurnState::OnExit() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);
}
    
void EnemyPlayTurnState::OnUpdate()
{
    Enemy& enemy = GetEnemy();
    Monster::Controller& controller = enemy.GetController();
    bool succeed = controller.ProcessAction(); // 컨트롤러가 액션을 마치면 true를 반환함.
    if (succeed)
    {
        enemy.EndTurn();
    }
}

void EnemyPlayTurnState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
    Enemy& enemy = GetEnemy();
    Monster::Controller& controller = enemy.GetController();
    controller.ProcessAnimationEvent(context);
}