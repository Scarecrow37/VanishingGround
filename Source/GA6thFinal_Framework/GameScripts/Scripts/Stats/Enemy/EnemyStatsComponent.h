#pragma once
#include "UmFramework.h"
class EnemyStatsComponent : public Component
{
    USING_PROPERTY(EnemyStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    EnemyStatsComponent();
    virtual ~EnemyStatsComponent();

protected:
    REFLECT_FIELDS_BEGIN(Component)
    REFLECT_FIELDS_END(EnemyStatsComponent)
};
