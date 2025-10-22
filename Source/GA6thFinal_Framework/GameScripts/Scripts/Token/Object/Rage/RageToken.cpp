#include "pchScripts.h"
#include "RageToken.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Token/TokenInventory.h"

namespace TokenObject
{
    REGISTER_TOKEN(Rage1)
    REGISTER_TOKEN(Rage2)
    void Rage::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                Enemy& target, EnemyStats& targetStats)
    {
        int   tokenID   = GetTokenID();
        int   param     = GetTokenParam(0);
        float factor    = static_cast<float>(param) / 100.0f;
        weaponStats.HitDamageMultiplier += factor;
        weaponStats.CriticalDamageMultiplier += factor;
        TokenInventory& tokenInventory = attacker.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
    void Rage::OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats,
                                                Player& target, PlayerStats& targetStats)
    {
        int   tokenID   = GetTokenID();
        int   param     = GetTokenParam(0);
        float factor    = static_cast<float>(param) / 100.0f;
        attackerStats.DamageMultiplier += factor;
        TokenInventory& tokenInventory = attacker.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
} // namespace TokenObject