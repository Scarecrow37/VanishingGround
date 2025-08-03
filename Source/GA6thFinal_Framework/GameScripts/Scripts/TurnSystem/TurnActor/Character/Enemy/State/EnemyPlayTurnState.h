#pragma once
#include "Base/EnemyStateBase.h"
#include "../Enum/EnemyEnum.h"


namespace EnemyAction
{
    class ActionBase;
}
class EnemyAI;

/*
자신의 턴이 온 상태입니다.
*/
class EnemyPlayTurnState : public EnemyStateBase
{
    using Action = EnemyAction::ActionBase;

public:
    void OnNotifiedAnimationEvent(const Timeline::EventContext* context) override;

private:
    // EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    /// <summary>
    /// Action 테이블을 초기화합니다.
    /// </summary>
    void ClearAction();

    /// <summary>
    /// EnemyType에 맞는 AIModel을 셋업합니다.
    /// </summary>
    /// <param name="type"></param>
    void SetAIModel(EnemyType type);

    /// <summary>
    /// EnemyType에 액션 함수를 바인드합니다.
    /// </summary>
    /// <param name="type"></param>
    void SetActions(EnemyType type);

    ///////////////////////////////////
    // MonsterA
    ///////////////////////////////////

    void BuildAIModel23000();
    void BuildAIModel23001();

    ///////////////////////////////////
    // MonsterB
    ///////////////////////////////////

    void BuildAIModel23010();
    void BuildAIModel23011();

    //================================
    // Condition
    //================================

    // 플레이어 출혈 여부(구현 X)
    bool IsPlayerBleeding();

    //================================
    // Debugging
    //================================
    
    // 액션 ID에 해당하는 액션 이름을 반환합니다.
    std::string_view GetActionName(int actionID) const;
    // 현재 액션을 로그로 출력합니다.
    void LogCurrentAction();

    void RequireCurrentAction();

private:
    Action* _currentAction  = nullptr;
    std::unordered_map<int, std::unique_ptr<Action>> _actionTable;

    REFLECT_FIELDS_BEGIN(EnemyStateBase)
    REFLECT_FIELDS_END(EnemyPlayTurnState)
};