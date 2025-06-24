#include "pchScripts.h"
#include "PlayerStatsComponent.h"
#include "PlayerStats.h"

PlayerStatsComponent::PlayerStatsComponent() = default;
PlayerStatsComponent::~PlayerStatsComponent() = default;

CharacterStats* PlayerStatsComponent::NewCharacterStats()
{
    _stats = new PlayerStats;
    return _stats;
}
