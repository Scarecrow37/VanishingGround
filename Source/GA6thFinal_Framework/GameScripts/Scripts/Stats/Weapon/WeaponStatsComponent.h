#pragma once
#include <Stats/TurnActorStatsComponent.h>

class WeaponStatsComponent : public TurnActorStatsComponent
{
    USING_PROPERTY(WeaponStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    WeaponStatsComponent();
    virtual ~WeaponStatsComponent();

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStatsComponent)
    REFLECT_FIELDS_END(WeaponStatsComponent)

    // TurnActorStatsComponent을(를) 통해 상속됨
    TurnActorStats* NewTurnActorStats() override;
};
