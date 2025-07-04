#include "pchScripts.h"
#include "PlayerStatsComponent.h"
#include "PlayerStats.h"

PlayerStatsComponent::PlayerStatsComponent() = default;
PlayerStatsComponent::~PlayerStatsComponent() = default;

TurnActorStats* PlayerStatsComponent::NewTurnActorStats()
{
    _stats = new PlayerStats;
    return _stats;
}
