#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define BUILD_AI(index)             \
case index:                         \
BuildAIModel##index();              \
break;

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define BUILD_ACTION(index)         \
_actionTable[index] = std::bind(&EnemyPlayTurnState::Action##index, this); \

// switch문 안에 있어야함
// Action ID값에 해당하는 함수 실행
#define PROCESS_ACTION(index)       \
case index:                         \
BuildAIModel##index();              \
break;

void EnemyPlayTurnState::OnAwake() 
{
}

void EnemyPlayTurnState::OnStart() 
{
    EnemyType type = GetEnemy().Type;
    SetAIModel(type);
    SetActions(type);
}

void EnemyPlayTurnState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    UmTime.Invoke(&GetFSM(), 2.f, [=]() { GetEnemy().EndTurn(); });

    LogCurrentAction();
}

void EnemyPlayTurnState::OnExit() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    _aiModel.Tansition();
    _aiModel.Refresh();
}
    
void EnemyPlayTurnState::OnUpdate()
{
    ProcessAction();
}

void EnemyPlayTurnState::ProcessAction() 
{
   
}

void EnemyPlayTurnState::SetAIModel(EnemyType type)
{
    _aiModel.Clear();
    switch (type)
    {
    case EnemyType::MONSTER_A: {
        int index = Random::Range(23000, 23000);
        switch (index)
        {
            BUILD_AI(23000)
            BUILD_AI(23001)
        default:
            break;
        }
        break;
    }
    case EnemyType::MONSTER_B: {
        int index = Random::Range(23010, 23011);
        switch (index)
        {
            BUILD_AI(23010)
            BUILD_AI(23011)
        default:
            break;
        }
        break;
        break;
    }
    case EnemyType::MONSTER_C: {
        break;
    }
    default:
        break;
    }
}

void EnemyPlayTurnState::SetActions(EnemyType type) 
{
    _actionTable.clear();
    switch (type)
    {
    case EnemyType::MONSTER_A: {
        BUILD_ACTION(22000)
        BUILD_ACTION(22001)
        BUILD_ACTION(22002)
        BUILD_ACTION(22003)
        BUILD_ACTION(22004)
        break;
    }
    case EnemyType::MONSTER_B: {
        BUILD_ACTION(22010)
        BUILD_ACTION(22011)
        BUILD_ACTION(22012)
        BUILD_ACTION(22013)
        BUILD_ACTION(22014)
        break;
    }
    case EnemyType::MONSTER_C: {
        break;
    }
    default:
        break;
    }
}

void EnemyPlayTurnState::BuildAIModel23000() 
{
    _aiModel.PushActionNode("#1", "#2", 22000); // Action 22000
    _aiModel.PushActionNode("#3", "#2", 22001); // Action 22001
    _aiModel.PushActionNode("#4", "#1", 22002); // Action 22002
    _aiModel.PushConditionNode("#2", "#3", "#4", std::bind(&EnemyPlayTurnState::IsPlayerBleeding, this)); // 조건 노드

    // Entry 노드 설정
    _aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::BuildAIModel23001() 
{
    _aiModel.PushActionNode("#1", "#2", 22004); // Action 22004
    _aiModel.PushActionNode("#2", "#3", {{50.0f, 22003}, {50.0f, 22000}}); // Action 22003, 22000
    _aiModel.PushActionNode("#3", "#4", 22002); // Action 22002
    _aiModel.PushActionNode("#4", "#1", {{50.0f, 22000}, {50.0f, 22002}}); // Action 22000, 22002

    // Entry 노드 설정
    _aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::Action22000()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message = std::format("{}: {}", gameObject->ToString(), (const char*)u8"찢 어 발 기 기 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22001()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"기 습 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22002()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"확 인 사 살 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22003()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"목 격 자 제 거 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22004()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"피 의 의 식 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::BuildAIModel23010() 
{
    _aiModel.PushActionNode("#1", "#2", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    _aiModel.PushActionNode("#2", "#3", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    _aiModel.PushActionNode("#3", "#4", 22013); // Action 22013
    _aiModel.PushActionNode("#4", "#3", {{15.0f, 22010}, {15.0f, 22011}, {70.0f, 22012}}); // Action 22010, 22011, 22012

    // Entry 노드 설정
    _aiModel.SetCurrentNode("#1"); 
}

void EnemyPlayTurnState::BuildAIModel23011() 
{
    _aiModel.PushActionNode("#1", "#2", 22014); // Action 22014
    _aiModel.PushActionNode("#2", "#3", {{25.0f, 22010}, {25.0f, 22011}, {50.0f, 22014}}); // Action 22010, 22011, 22014
    _aiModel.PushActionNode("#3", "#4", 22013); // Action 22013
    _aiModel.PushActionNode("#4", "#3", {{30.0f, 22011}, {70.0f, 22012}}); // Action 22011, 22012

    // Entry 노드 설정
    _aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::Action22010()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"두 려 움 의 중 얼 거 림 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22011()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"떨 리 는 중 얼 거 림 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22012()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"절 망 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22013()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"소 멸 의 찬 가 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

void EnemyPlayTurnState::Action22014()
{
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{}: {}", gameObject->ToString(), (const char*)u8"웅 크 리 기 !!!");
    UmLogger.Message(LogLevel::LEVEL_WARNING, message);
}

bool EnemyPlayTurnState::IsPlayerBleeding()
{
    // TODO: 실제 출혈 상태를 확인하는 로직을 구현해야 합니다.
    return 0 == Random::Range(0 , 1);
}

void EnemyPlayTurnState::LogCurrentAction() 
{
    int actionID = _aiModel.GetCurrentActionID();
    auto actionIt = _actionTable.find(actionID);
    if (actionIt != _actionTable.end())
    {
        actionIt->second(); // 액션 실행
    }
    else
    {
        GameObject* gameObject = &GetFSM().gameObject;
        std::string message = std::format("{}: {}", gameObject->ToString(), (const char*)u8"알 수 없는 액션 ID입니다.");
        UmLogger.Message(LogLevel::LEVEL_ERROR, message);
    }
}
