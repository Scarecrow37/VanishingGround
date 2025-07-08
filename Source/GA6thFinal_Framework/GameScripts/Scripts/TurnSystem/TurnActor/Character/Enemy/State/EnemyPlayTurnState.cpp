#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>
#include "../AI/23000/EnemyAI23000.h"

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

void EnemyPlayTurnState::OnAwake() 
{
}

void EnemyPlayTurnState::OnStart() 
{
    EnemyType type = GetEnemy().Type;
    SetAIModel(type);
}

void EnemyPlayTurnState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    UmTime.Invoke(&GetFSM(), 1.f, [=]() { GetEnemy().EndTurn(); });

    _aiModel->Process();
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

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define REGISTER_AI(index)          \
case 23000:                         \
_aiModel = new EnemyAI##index();    \
break;

void EnemyPlayTurnState::SetAIModel(EnemyType type)
{
    switch (type)
    {
    case EnemyType::MONSTER_A: {
        int index = Random::Range(23000, 23000);
        switch (index)
        {
            REGISTER_AI(23000)
        default:
            break;
        }
        break;
    }
    case EnemyType::MONSTER_B: {
        break;
    }

    case EnemyType::MONSTER_C: {
        break;
    }
    default:
        break;
    }
}

bool EnemyPlayTurnState::ProcessAI(int aiModelID)
{
    return false;
}

bool EnemyPlayTurnState::Action22000()
{
    return false;
}

bool EnemyPlayTurnState::Action22001()
{
    return false;
}

bool EnemyPlayTurnState::Action22002()
{
    return false;
}

bool EnemyPlayTurnState::Action22003()
{
    return false;
}

bool EnemyPlayTurnState::Action22004()
{
    return false;
}