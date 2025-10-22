#pragma once
#include "Stats/TurnActorStatsComponent.h"

struct EnemyStats;
class EnemyStatsComponent : public TurnActorStatsComponent
{
    USING_PROPERTY(EnemyStatsComponent)
public:
    REFLECT_PROPERTY()

public:
    EnemyStatsComponent();
    virtual ~EnemyStatsComponent();

    EnemyStats& GetStats() { return *_stats; }

    void RegisterHUD(const std::string& key) const;

protected:
    REFLECT_FIELDS_BEGIN(TurnActorStatsComponent)
    REFLECT_FIELDS_END(EnemyStatsComponent)

    // CharacterStatsComponent을(를) 통해 상속됨
    TurnActorStats* NewTurnActorStats() override;

private:
    EnemyStats* _stats = nullptr;
};
