#pragma once
#include "../FSMState.h"

class FSMStateFactory : public FactoryConstructor<FSMState>
{
public:
    FSMStateFactory() = default;
    virtual ~FSMStateFactory() override = default;
};