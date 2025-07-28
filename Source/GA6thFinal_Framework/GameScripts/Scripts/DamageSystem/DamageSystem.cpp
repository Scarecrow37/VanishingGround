#include "pchScripts.h"
#include "DamageSystem.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Weapon/WeaponStats.h>
#include <EnemyAction/System/EnemyActionSystem.h>

int DamageSystem::CalculateDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int   hitDamage                = attacker._weaponStats.CriticalDamage;
    float criticalDamageMultiplier = attacker._weaponStats.CriticalDamageMultiplier;
    hitDamage                      = static_cast<int>(std::round(hitDamage * criticalDamageMultiplier));
    return hitDamage;
}

int DamageSystem::CalculateChainDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int   chainDamage           = attacker._weaponStats.AttackPerChain;
    float chainDamageMultiplier = attacker._weaponStats.AttackPerChainMultiplier;
    chainDamage               = static_cast<int>(std::round(chainDamage * chainDamageMultiplier));
    return chainDamage;
}

int DamageSystem::CalculateDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    int   hitDamage                = attacker._enemyStats.Damage;
    float criticalDamageMultiplier = attacker._enemyStats.DamageMultiplier;
    hitDamage                      = static_cast<int>(std::round(hitDamage * criticalDamageMultiplier));
    return hitDamage;
}

int DamageSystem::CalculateChainDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    return 1;
}
