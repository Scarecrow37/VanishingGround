#pragma once
#include "Base/EnemyStateBase.h"
#include "../Enum/EnemyEnum.h"
#include "../AI/EnemyAI.h"

class EnemyAI;

/*
자신의 턴이 온 상태입니다.
*/
class EnemyPlayTurnState : public EnemyStateBase
{
private:
    // EnemyStateBase을(를) 통해 상속됨
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    void ProcessAction();

private:
    void SetAIModel(EnemyType type);
    void SetActions(EnemyType type);

    ///////////////////////////////////
    // MonsterA
    ///////////////////////////////////
    void BuildAIModel23000();
    void BuildAIModel23001();
    void Action22000();
    void Action22001();
    void Action22002();
    void Action22003();
    void Action22004();

    ///////////////////////////////////
    // MonsterB Action
    ///////////////////////////////////
    void BuildAIModel23010();
    void BuildAIModel23011();
    void Action22010();
    void Action22011();
    void Action22012();
    void Action22013();
    void Action22014();

    //================================
    // Condition
    //================================
    bool IsPlayerBleeding();

    //================================
    // Debugging
    //================================
    void LogCurrentAction();

private:
    EnemyAI _aiModel;
    std::unordered_map<int, std::function<void()>> _actionTable;

    REFLECT_FIELDS_BEGIN(EnemyStateBase)
    int AIModelID = 0; // AI 모델 ID
    REFLECT_FIELDS_END(EnemyPlayTurnState)
};