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

    void Rage::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage)
    {
        const int   tokenID            = GetTokenID();
        const int   param              = GetTokenParam(0);
        const float factor             = 1.0f + (static_cast<float>(param) / 100.0f);
        const float damageFloat        = static_cast<float>(damage) * factor;
        damage                         = static_cast<int>(std::ceilf(damageFloat));
        auto& tokenInventory           = attackerData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
    void Rage::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)
    {
        const int   tokenID            = GetTokenID();
        const int   param              = GetTokenParam(0);
        const float factor             = 1.0f + (static_cast<float>(param) / 100.0f);
        const float damageFloat        = static_cast<float>(damage) * factor;
        damage                         = static_cast<int>(std::ceilf(damageFloat));
        auto& tokenInventory           = attackerData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
} // namespace TokenObject