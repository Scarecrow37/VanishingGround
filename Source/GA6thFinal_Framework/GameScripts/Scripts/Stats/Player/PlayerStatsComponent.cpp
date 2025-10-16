#include "pchScripts.h"
#include "PlayerStatsComponent.h"
#include "PlayerStats.h"


UMREAL_COMPONENT(PlayerStatsComponent)

PlayerStatsComponent::PlayerStatsComponent() = default;
PlayerStatsComponent::~PlayerStatsComponent() = default;

void PlayerStatsComponent::RegisterHUD() const
{
    if (nullptr != _stats)
        _stats->RegisterHUD();
}

TurnActorStats* PlayerStatsComponent::NewTurnActorStats()
{
    _stats = new PlayerStats;
    return _stats;
}