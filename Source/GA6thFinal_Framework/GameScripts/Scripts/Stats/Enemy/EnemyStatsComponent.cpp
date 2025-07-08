#include "pchScripts.h"
#include "EnemyStats.h"
#include "EnemyStatsComponent.h"
EnemyStatsComponent::EnemyStatsComponent() = default;
EnemyStatsComponent::~EnemyStatsComponent() = default;

TurnActorStats* EnemyStatsComponent::NewTurnActorStats()
{
    _stats = new EnemyStats;
    return _stats;
}
