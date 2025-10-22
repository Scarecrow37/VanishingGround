#include "pchScripts.h"
#include "VanishToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Weapon/WeaponStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Vanish)

    void Vanish::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                  WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                  Enemy& target, EnemyStats& targetStats)
    {
        int   param  = GetTokenParam(0);
        float factor = static_cast<float>(param) / 100.0f;
        weaponStats.HitDamageMultiplier += factor;
        weaponStats.CriticalDamageMultiplier += factor;
    }

    void Vanish::OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                  PlayerStats& targetStats)
    {
        int   param  = GetTokenParam(0);
        float factor = static_cast<float>(param) / 100.0f;
        attackerStats.DamageMultiplier += factor;
    }
} // namespace TokenObject