#pragma once
#include "../FSMCondition.h"

class FSMConditionFactory : public FactoryConstructor<FSMCondition>
{
public:
    FSMConditionFactory() = default;
    virtual ~FSMConditionFactory() override = default;
};