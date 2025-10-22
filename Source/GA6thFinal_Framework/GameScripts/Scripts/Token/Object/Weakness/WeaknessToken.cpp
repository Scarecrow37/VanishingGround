#include "pchScripts.h"
#include "WeaknessToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Stats/Enemy/EnemyStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Weakness1)
    REGISTER_TOKEN(Weakness2)
    REGISTER_TOKEN(Weakness3)

    void Weakness::OnTurnEnd(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
    void Weakness::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                    WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                    Enemy& target, EnemyStats& targetStats)
    {
        int   tokenID = GetTokenID();
        int   param   = GetTokenParam(0);
        float factor  = static_cast<float>(param) / 100.0f;
        weaponStats.HitDamageMultiplier -= factor;
        weaponStats.CriticalDamageMultiplier -= factor;
    }
    void Weakness::OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                    PlayerStats& targetStats)
    {
        int   tokenID = GetTokenID();
        int   param   = GetTokenParam(0);
        float factor  = static_cast<float>(param) / 100.0f;
        attackerStats.DamageMultiplier -= factor;
    }
}
