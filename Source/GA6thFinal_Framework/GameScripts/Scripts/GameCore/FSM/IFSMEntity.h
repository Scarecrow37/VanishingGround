#pragma once
#include "UmFramework.h"

class IFSMEntity : public ReflectSerializer
{
    friend class FiniteStateMachine;
public:
    IFSMEntity() = default;
    virtual ~IFSMEntity() = default;

    /// <summary>
    /// FSM의 Awake 호출시 호출됩니다.
    /// </summary>
    virtual void OnAwake() = 0;

    /// <summary>
    /// FSM의 Start 호출시 호출됩니다.
    /// </summary>
    virtual void OnStart() = 0;

    inline FiniteStateMachine& GetFSM() { &_owner; }
private:
    FiniteStateMachine* _owner = nullptr;
};