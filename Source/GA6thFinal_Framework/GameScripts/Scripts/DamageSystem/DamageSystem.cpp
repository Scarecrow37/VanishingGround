#include "pchScripts.h"
#include "DamageSystem.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Weapon/WeaponStats.h>

int DamageSystem::CalculateDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int hitDamage = attacker._weaponStats.HitDamage;
    float criticalDamageMultiplier = attacker._weaponStats.CriticalDamageMultiplier;
    hitDamage = static_cast<int>(std::round(hitDamage * criticalDamageMultiplier));
    return hitDamage;
}

int DamageSystem::CalculateChainDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int chainDamage = attacker._weaponStats.AttackPerChain;
    return chainDamage;
}

int DamageSystem::CalculateDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    int result = 0;
    result = 10;
    return result;
}

int DamageSystem::CalculateChainDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    return 1;
}
