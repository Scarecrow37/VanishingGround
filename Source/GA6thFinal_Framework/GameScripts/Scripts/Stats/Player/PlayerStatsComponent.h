#pragma once
#include "Stats/TurnActorStatsComponent.h"

struct PlayerStats;
class PlayerStatsComponent : public TurnActorStatsComponent
{
    USING_PROPERTY(PlayerStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    PlayerStatsComponent();
    virtual ~PlayerStatsComponent();

public:
    PlayerStats& GetStats() { return *_stats; }

public:
    void RegisterHUD() const;

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStatsComponent)
    REFLECT_FIELDS_END(PlayerStatsComponent)

    // 이 함수를 override 해서 CharacterStats를 동적할당해 반환해야 합니다.
    TurnActorStats* NewTurnActorStats() override;

private:
    PlayerStats* _stats = nullptr;

};
