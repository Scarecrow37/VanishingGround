#pragma once
#include "UmFramework.h"

class TurnActorStats : public ReflectSerializer
{
    USING_PROPERTY(TurnActorStats)
public:
    REFLECT_PROPERTY()
    TurnActorStats() = default;
    ~TurnActorStats() override = default;

protected:
    REFLECT_FIELDS_BEGIN(ReflectSerializer)

    REFLECT_FIELDS_END(TurnActorStats)

};
