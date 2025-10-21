#pragma once
#include "IFSMEntity.h"

class FSMState : public IFSMEntity
{
public:
    FSMState() = default;
    virtual ~FSMState() override = default;

    /// <summary>
    /// 상태 진입시 호출됩니다.
    /// </summary>
    virtual void OnEnter()  = 0;
    /// <summary>
    /// 상태 퇴출시 호출됩니다
    /// </summary>
    virtual void OnExit()   = 0;
    /// <summary>
    /// 상태가 유지되는 동안 매 프레임 호출됩니다.
    /// </summary>
    virtual void OnUpdate() = 0;

    REFLECT_FIELDS_BEGIN(IFSMEntity)
    REFLECT_FIELDS_END(FSMState)

};

template <typename T>
concept FSM_STATE_BASE = std::is_base_of_v<FSMState, T>;