#include "pchScripts.h"
#include "DamageSystem.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Weapon/WeaponStats.h>

int DamageSystem::CalculateDamage(const PlayerInfo& attacker, const EnemyInfo& target)
{
    int result = 0;
    result     = attacker._weaponStats.CriticalDamage;
    return result;
}

int DamageSystem::CalculateDamage(const EnemyInfo& attacker, const PlayerInfo& target)
{
    int result = 0;
    result = 10;
    return result;
}
