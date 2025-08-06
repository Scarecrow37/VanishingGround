#include "pchScripts.h"
#include "EnemyStats.h"
#include "EnemyStatsComponent.h"
EnemyStatsComponent::EnemyStatsComponent() = default;
EnemyStatsComponent::~EnemyStatsComponent() = default;

void EnemyStatsComponent::RegisterHP(const std::string& key) const
{
    if (nullptr != _stats)
        _stats->RegisterHP(key);
}

TurnActorStats* EnemyStatsComponent::NewTurnActorStats()
{
    _stats = new EnemyStats;
    return _stats;
}
