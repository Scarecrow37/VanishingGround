#pragma once
#include "UmFramework.h"

class FiniteStateMachine;
class TurnActor;
class Enemy;

/*
* 턴을 관리하는 컴포넌트입니다.
*/
class TurnMode : public Component
{
    USING_PROPERTY(TurnMode)
public:
    REFLECT_PROPERTY(
        RoundCount
    )

public:
    TurnMode();
    virtual ~TurnMode();

public:
    FiniteStateMachine& GetFSM() { return *_finiteStateMachine; }

public:
    /// <summary>
    /// 라운드를 증가시킵니다.
    /// </summary>
    /// <returns></returns>
    int AddRoundCount() { return ++_roundCount; }

    /// <summary>
    /// 현재 Scene에 존재하는 모든 TurnActor를 TurnList에 담습니다.
    /// </summary>
    void MakeTurnList();

    /// <summary>
    /// Random Speed를 뽑고 TurnList를 정렬합니다.
    /// </summary>
    void SortTurnList();

    /// <summary>
    /// 가장 우선순위가 높은 TurnActor를 List에서 지우고 턴을 실행합니다.
    /// </summary>
    /// <returns></returns>
    TurnActor* PopTurnList();

public:
    GETTER_ONLY(int, RoundCount) { return _roundCount; }
    PROPERTY(RoundCount)

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnMode)

private:
    void BuildTurnModeFSM();

private:
    FiniteStateMachine* _finiteStateMachine   = nullptr;

    int _roundCount;
    std::deque<TurnActor*> _turnList;
    TurnActor* _currTurnActor;

private:
    struct SystemStates
    {
        class CombatStartPhase* CombatStartPhase = nullptr;
        class RoundStartPhase*  RoundStartPhase  = nullptr;
    } _systemStates;

    struct SystemCondition
    {
        class CombatStartCodition* CombatStartCodition = nullptr;
        class RoundStartCondition* RoundStartCondition = nullptr;
    } _systemConditions;

public:
    GETTER_ONLY(const SystemStates&, States) { return _systemStates; }
    /// <summary>
    /// TurnMode FSM의 State 객체들 입니다.
    /// </summary>
    PROPERTY(States)

    /// <summary>
    /// TurnMode용 FSM의 Condition 객체들 입니다.
    /// </summary>
    GETTER_ONLY(const SystemCondition&, Conditions) { return _systemConditions; }
    PROPERTY(Conditions)

protected:
    /// <summary>
    /// <para> 이 함수는 항상 Start 함수 전에 호출되며 프리팹이 인스턴스화 된 직후에 호출됩니다.                </para>
    /// <para> 게임 오브젝트의 Active가 false 상태인 경우 Awake 함수는 true가 될때까지 호출되지 않습니다.      </para>
    /// </summary>
    virtual void Awake();

    virtual void ImGuiDrawPropertysEvent() override;
};
