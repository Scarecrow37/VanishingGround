#include "pchScripts.h"
#include "EnemyStats.h"
#include "EnemyStatsComponent.h"


UMREAL_COMPONENT(EnemyStatsComponent)

EnemyStatsComponent::EnemyStatsComponent() = default;
EnemyStatsComponent::~EnemyStatsComponent() = default;

void EnemyStatsComponent::RegisterHUD(const std::string& key) const
{
    if (nullptr != _stats)
        _stats->RegisterHUD(key);
}

TurnActorStats* EnemyStatsComponent::NewTurnActorStats()
{
    _stats = new EnemyStats;
    return _stats;
}
