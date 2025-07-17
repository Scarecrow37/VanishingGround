#include "pchScripts.h"
#include "DamageSystem.h"
#include <Stats/Player/PlayerStats.h>
#include <Stats/Enemy/EnemyStats.h>
#include <Stats/Weapon/WeaponStats.h>

int DamageSystem::CalculateDamage(const PlayerStats& attacker, const WeaponStats& attackerWeapon, const EnemyStats& target)
{
    int result = 0;
    result = attackerWeapon.CriticalDamage;
    return result;
}
