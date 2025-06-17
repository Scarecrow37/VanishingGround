#pragma once
#include "UmFramework.h"
class TurnActor : public Component
{
    USING_PROPERTY(TurnActor)
public:
    REFLECT_PROPERTY()

public:
    TurnActor();
    virtual ~TurnActor();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(TurnActor)
};
