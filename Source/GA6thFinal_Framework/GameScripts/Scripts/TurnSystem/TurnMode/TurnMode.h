#pragma once
#include "UmFramework.h"
class FiniteStateMachine;

class TurnMode : public Component
{
    USING_PROPERTY(TurnMode)
public:
    REFLECT_PROPERTY()

public:
    TurnMode();
    virtual ~TurnMode();

    FiniteStateMachine& GetFSM() { return *finiteStateMachine; }

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnMode)

private:
    FiniteStateMachine* finiteStateMachine = nullptr;
};
