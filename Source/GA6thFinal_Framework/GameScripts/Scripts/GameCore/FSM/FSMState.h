#pragma once
#include <UmFramework.h>

class FSMState : public ReflectSerializer
{
public:
    FSMState() = default;
    virtual ~FSMState() = default;

    REFLECT_FIELDS_BEGIN(ReflectSerializer)
    REFLECT_FIELDS_END(FSMState)
};