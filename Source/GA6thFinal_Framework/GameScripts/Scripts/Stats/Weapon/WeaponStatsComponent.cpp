#include "pchScripts.h"
#include "WeaponStatsComponent.h"
#include <Stats/Weapon/WeaponStats.h>

WeaponStatsComponent::WeaponStatsComponent() = default;
WeaponStatsComponent::~WeaponStatsComponent() = default;

TurnActorStats* WeaponStatsComponent::NewTurnActorStats()
{
    return new WeaponStats;
}
