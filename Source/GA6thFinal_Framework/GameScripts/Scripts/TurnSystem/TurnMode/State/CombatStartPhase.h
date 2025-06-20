#pragma once
#include "GameCore/FSM/FiniteStateMachine.h"

class CombatStartPhase : public FSMState
{
public:
    CombatStartPhase();
    virtual ~CombatStartPhase() override;

    bool IsEndPhase() const { return _phaseEnd; }
    
public:
    /// <summary>
    /// 캐릭터들의 스텟을 초기화합니다.
    /// </summary>
    void ResetCharacterStats();

protected:
    void OnAwake() override;
    void OnStart() override;
    void OnEnter() override;
    void OnExit() override;
    void OnUpdate() override;

private:
    bool _phaseEnd;
};