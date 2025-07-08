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

    // 찢어 발기기(구현 X)
    void Action22000(); 
    // 기습(구현 X)
    void Action22001();
    // 확인 사살(구현 X)
    void Action22002(); 
    // 연기(?) 베기(구현 X)
    void Action22003(); 
    // 피의 의식(구현 X)
    void Action22004(); 

    ///////////////////////////////////
    // MonsterB
    ///////////////////////////////////

    void BuildAIModel23010();
    void BuildAIModel23011();

    // 두려움의 중얼거림(구현 X)
    void Action22010(); 
    // 떨리는 중얼거림(구현 X)
    void Action22011(); 
    // 절망(구현 X)
    void Action22012(); 
    // 소멸의 찬가(구현 X)
    void Action22013(); 
    // 웅크리기(구현 X)
    void Action22014();

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

private:
    EnemyAI _aiModel;
    std::unordered_map<int, std::function<void()>> _actionTable;

    REFLECT_FIELDS_BEGIN(EnemyStateBase)
    REFLECT_FIELDS_END(EnemyPlayTurnState)
};