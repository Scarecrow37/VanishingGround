#include "pchScripts.h"
#include "DamageSystem.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Weapon/WeaponStats.h>

int DamageSystem::CalculateDamage(const PlayerInfo& attacker, const EnemyInfo& target, const QTE::NoteResult& result)
{
    int   hitDamage = 0;
    float criticalDamageMultiplier = 1.0f;
    switch (result.Result)
    {
        case QTE::ResultType::QTE_RESULT_PERFECT: {
            hitDamage                = attacker._weaponStats.CriticalDamage;
            criticalDamageMultiplier = attacker._weaponStats.CriticalDamageMultiplier;
            break;
        }
        case QTE::ResultType::QTE_RESULT_NORMAL: {
            hitDamage                = attacker._weaponStats.HitDamage;
            criticalDamageMultiplier = attacker._weaponStats.HitDamageMultiplier;
            break;
        }
        case QTE::ResultType::QTE_RESULT_MISS: {
            break;
        }
    default:
        break;
    }
    hitDamage                   = std::max(hitDamage, 1);
    criticalDamageMultiplier    = std::max(criticalDamageMultiplier, 0.0f);
    hitDamage                   = static_cast<int>(std::round(hitDamage * criticalDamageMultiplier));
    return hitDamage;
}

int DamageSystem::CalculateChainDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int   chainDamage           = attacker._weaponStats.AttackPerChain;
    float chainDamageMultiplier = attacker._weaponStats.AttackPerChainMultiplier;
    chainDamage                 = std::max(chainDamage, 1);
    chainDamageMultiplier       = std::max(chainDamageMultiplier, 0.0f);
    chainDamage = static_cast<int>(std::round(chainDamage * chainDamageMultiplier));
    return chainDamage;
}

int DamageSystem::CalculateDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    int   hitDamage                = attacker._enemyStats.Damage;
    float criticalDamageMultiplier = attacker._enemyStats.DamageMultiplier;
    hitDamage                      = std::max(hitDamage, 1);
    criticalDamageMultiplier       = std::max(criticalDamageMultiplier, 0.0f);
    hitDamage                      = static_cast<int>(std::round(hitDamage * criticalDamageMultiplier));
    return hitDamage;
}

int DamageSystem::CalculateChainDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    return 1;
}
