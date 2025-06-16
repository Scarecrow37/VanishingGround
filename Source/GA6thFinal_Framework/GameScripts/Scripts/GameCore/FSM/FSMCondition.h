#pragma once
#include "UmFramework.h"

class FSMCondition : public ReflectSerializer
{
public:
    FSMCondition() = default;
    virtual ~FSMCondition() override = default;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(FSMCondition)
};