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

    auto& enemy = GetEnemy();
    enemy.GetTokenSystem().OnTurnStart(&enemy);

    ProcessAction();
}

void EnemyPlayTurnState::OnExit() 
{
    GameObject* gameObject = &GetFSM().gameObject;

    std::string message = std::format("{} {}", gameObject->ToString(), (const char*)u8"턴 종료.");
    UmLogger.Message(LogLevel::LEVEL_TRACE, message);

    auto& enemy = GetEnemy();
    enemy.GetTokenSystem().OnTurnEnd(&enemy);

    // Enemy의 턴이 종료시 액션을 선언.
    _aiModel.Transition();
    _aiModel.Refresh();
}
    
void EnemyPlayTurnState::OnUpdate()
{
    //ProcessAction();
}

void EnemyPlayTurnState::ProcessAction() 
{
    int  actionID = _aiModel.GetCurrentActionID();
    auto actionIt = _actionTable.find(actionID);
    if (actionIt != _actionTable.end())
    {
        actionIt->second(); // 액션 실행
    }
}

void EnemyPlayTurnState::SetAIModel(EnemyType type)
{
    _aiModel.Clear();
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

#include <TurnSystem/TurnActor/Character/Player/Player.h>
#include <Token/Object/BleedToken.h>
void EnemyPlayTurnState::Action22000()
{
    auto player = GameObject::FindGameObjectsWithTag(Player::TAG);
    if (false == player.empty())
    {
        auto& wpPlayer = player.front();
        if (false == wpPlayer.expired())
        {
            auto object = wpPlayer.lock();
            if (object)
            {
                auto playerComp = object->GetComponent<Player>();
                if (playerComp)
                {
                    // 플레이어에게 출혈 토큰을 추가합니다.
                    playerComp->GetTokenSystem().AddTokenStackFromID(BleedToken::ID, 1);
                }
            }
        }
    }
}

void EnemyPlayTurnState::Action22001()
{
}

void EnemyPlayTurnState::Action22002()
{
}

void EnemyPlayTurnState::Action22003()
{
}

void EnemyPlayTurnState::Action22004()
{
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
}

void EnemyPlayTurnState::Action22011()
{
}

void EnemyPlayTurnState::Action22012()
{
}

void EnemyPlayTurnState::Action22013()
{
}

void EnemyPlayTurnState::Action22014()
{
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
    int actionID = _aiModel.GetCurrentActionID();
    GameObject* gameObject = &GetFSM().gameObject;
    std::string message = std::format("{} {}", gameObject->ToString(), GetActionName(actionID));
    UmLogger.Message(LogLevel::LEVEL_DEBUG, message);
}