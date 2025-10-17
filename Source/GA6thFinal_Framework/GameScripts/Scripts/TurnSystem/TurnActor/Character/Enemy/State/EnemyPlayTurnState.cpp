#include "pchScripts.h"
#include "EnemyPlayTurnState.h"

#include <TurnSystem/TurnActor/Character/Enemy/Enemy.h>

#include <EnemyAction/Action/22000_22009/EnemyAction22000.h>
#include <EnemyAction/Action/22000_22009/EnemyAction22001.h>
#include <EnemyAction/Action/22000_22009/EnemyAction22002.h>
#include <EnemyAction/Action/22000_22009/EnemyAction22003.h>
#include <EnemyAction/Action/22000_22009/EnemyAction22004.h>

#include <EnemyAction/Action/22010_22019/EnemyAction22010.h>
#include <EnemyAction/Action/22010_22019/EnemyAction22011.h>
#include <EnemyAction/Action/22010_22019/EnemyAction22012.h>
#include <EnemyAction/Action/22010_22019/EnemyAction22013.h>
#include <EnemyAction/Action/22010_22019/EnemyAction22014.h>

REGISTER_CLASS(FSMStateFactory, EnemyPlayTurnState)

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
    EnemyType type = GetEnemy().Type;
    SetAIModel(type);
    SetActions(type);
}

void EnemyPlayTurnState::OnEnter() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 시작.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    RequireCurrentAction();
    if (_currentAction)
    {
        _currentAction->RequestActionEnter();
    }
    LogCurrentAction();
}

void EnemyPlayTurnState::OnExit() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    if (_currentAction)
    {
        _currentAction->RequestActionExit();
    }
}
    
void EnemyPlayTurnState::OnUpdate()
{
    bool     result  = true;
    Enemy&   enemy   = GetEnemy();
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    if (_currentAction)
    {
        _currentAction->RequestActionUpdate();
        result = _currentAction->IsActionEnd();
    }
    if (true == result)
    {
        enemy.EndTurn();
    }
}

void EnemyPlayTurnState::OnNotifiedAnimationEvent(const Timeline::EventContext* context) 
{
    if (_currentAction)
    {
        _currentAction->OnAnimationEvent(context);
    }
}

void EnemyPlayTurnState::ClearAction() 
{
    _currentAction  = nullptr;
    _actionTable.clear();
}

void EnemyPlayTurnState::SetAIModel(EnemyType type)
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    aiModel.Clear();
    switch (type)
    {
    case EnemyType::MONSTER_A: {
        int index = Random::Range(23000, 23001);
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
    }
    case EnemyType::MONSTER_C: {
        break;
    }
    default:
        break;
    }
}

// switch문 안에 있어야함
// AI 모델 ID값에 해당하는 AI 모델을 셋업
#define BUILD_ACTION(index)         \
_actionTable[index] = std::make_unique<EnemyAction::Action##index>(&enemy); \

void EnemyPlayTurnState::SetActions(EnemyType type) 
{
    Enemy& enemy = GetEnemy();
    ClearAction();
    
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
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    aiModel.PushActionNode("#1", "#2", 22000);  // Action 22000
    aiModel.PushActionNode("#3", "#2", 22001);  // Action 22001
    aiModel.PushActionNode("#4", "#1", 22002);  // Action 22002
    aiModel.PushConditionNode("#2", "#3", "#4", std::bind(&EnemyPlayTurnState::IsPlayerBleeding, this)); // 조건 노드

    // Entry 노드 설정
    aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::BuildAIModel23001() 
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    aiModel.PushActionNode("#1", "#2", 22004); // Action 22004
    aiModel.PushActionNode("#2", "#3", {{50.0f, 22003}, {50.0f, 22000}}); // Action 22003, 22000
    aiModel.PushActionNode("#3", "#4", 22002); // Action 22002
    aiModel.PushActionNode("#4", "#1", {{50.0f, 22000}, {50.0f, 22002}}); // Action 22000, 22002

    // Entry 노드 설정
    aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::BuildAIModel23010() 
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    //aiModel.PushActionNode("#1", "#2", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    //aiModel.PushActionNode("#2", "#3", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    //aiModel.PushActionNode("#3", "#4", 22013); // Action 22013
    //aiModel.PushActionNode("#4", "#3", {{15.0f, 22010}, {15.0f, 22011}, {70.0f, 22012}}); // Action 22010, 22011, 22012

    aiModel.PushActionNode("#1", "#2", 22012); // Action 22012
    aiModel.PushActionNode("#2", "#3", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    aiModel.PushConditionNode("#3", "#4", "#1", [this]() -> bool { // HP가 50% 이하일 때
        auto& enemy  = GetEnemy();
        bool  result = enemy.HP / enemy.MaxHP <= 0.5f;
        return result;
    });
    aiModel.PushActionNode("#4", "#2", 22013); // Action 22013

    // Entry 노드 설정
    aiModel.SetCurrentNode("#1");
}

void EnemyPlayTurnState::BuildAIModel23011() 
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    //aiModel.PushActionNode("#1", "#2", 22014); // Action 22014
    //aiModel.PushActionNode("#2", "#3", {{25.0f, 22010}, {25.0f, 22011}, {50.0f, 22014}}); // Action 22010, 22011, 22014
    //aiModel.PushActionNode("#3", "#4", 22013); // Action 22013
    //aiModel.PushActionNode("#4", "#3", {{30.0f, 22011}, {70.0f, 22012}}); // Action 22011, 22012

    aiModel.PushActionNode("#1", "#2", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    aiModel.PushActionNode("#2", "#3", {{50.0f, 22010}, {50.0f, 22011}}); // Action 22010, 22011
    aiModel.PushConditionNode("#3", "#4", "#2", [this]() -> bool {  // HP가 50% 이하일 때
        auto& enemy = GetEnemy();
        bool result = enemy.HP / enemy.MaxHP <= 0.5f; 
        return result;
    });
    aiModel.PushActionNode("#4", "#2", 22013); // Action 22013

    // Entry 노드 설정
    aiModel.SetCurrentNode("#1");
}

bool EnemyPlayTurnState::IsPlayerBleeding()
{
    // TODO: 실제 출혈 상태를 확인하는 로직을 구현해야 합니다.
    return 0 == Random::Range(0 , 1);
}

#define GET_ACTION_NAME(actionID, str)  \
case actionID:                          \
actionName = (const char*)str;          \
break;

std::string_view EnemyPlayTurnState::GetActionName(int actionID) const
{
    static std::string actionName;
    switch (actionID)
    {
        GET_ACTION_NAME(0,     u8"유효하지 않은 액션 ID")
        GET_ACTION_NAME(22000, u8"찢어 발기기")
        GET_ACTION_NAME(22001, u8"기습")
        GET_ACTION_NAME(22002, u8"확인 사살")
        GET_ACTION_NAME(22003, u8"목격자 제거")
        GET_ACTION_NAME(22004, u8"피의 의식")
        GET_ACTION_NAME(22010, u8"두려움의 중얼거림")
        GET_ACTION_NAME(22011, u8"떨리는 중얼거림")
        GET_ACTION_NAME(22012, u8"절망")
        GET_ACTION_NAME(22013, u8"소멸의 찬가")
        GET_ACTION_NAME(22014, u8"웅크리기")
    default:
        break;
    }
    return actionName;
}

void EnemyPlayTurnState::LogCurrentAction()
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    int actionID = aiModel.GetCurrentActionID();
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message    = std::format("{} {}", gameObject->ToString(), GetActionName(actionID));
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);
}

void EnemyPlayTurnState::RequireCurrentAction() 
{
    EnemyAI& aiModel = GetEnemy().GetAIModel();
    int  actionID = aiModel.GetCurrentActionID();
    auto actionIt = _actionTable.find(actionID);
    if (actionIt != _actionTable.end())
    {
        _currentAction = actionIt->second.get();
    }
    else
    {
        _currentAction = nullptr;
    }
}
