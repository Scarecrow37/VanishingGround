#include "pchScripts.h"
#include "EnemyStats.h"
#include "EnemyStatsComponent.h"
EnemyStatsComponent::EnemyStatsComponent() = default;
EnemyStatsComponent::~EnemyStatsComponent() = default;

CharacterStats* EnemyStatsComponent::NewCharacterStats()
{
    _stats = new EnemyStats;
    return _stats;
}
