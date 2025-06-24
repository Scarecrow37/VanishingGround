#include "pchScripts.h"
#include "PlayerStatsComponent.h"
#include "PlayerStats.h"

PlayerStatsComponent::PlayerStatsComponent() = default;
PlayerStatsComponent::~PlayerStatsComponent() = default;

CharacterStats* PlayerStatsComponent::NewCharacterStats()
{
    return new PlayerStats;
}

void CharacterStatsComponent::Reset()
{
    CharacterStats* newStats = NewCharacterStats();
    _stats.reset(newStats);
}