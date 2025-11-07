#include "pchScripts.h"
#include "WeaknessToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Stats/Enemy/EnemyStats.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Weakness1)
    REGISTER_TOKEN(Weakness2)
    REGISTER_TOKEN(Weakness3)

    void Weakness::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                     int& damage)
    {
        const int   tokenID     = GetTokenID();
        const int   param       = GetTokenParam(0);
        auto& tokenInventory = attackerData.Source.GetTokenInventory();
        // 자신보다 높은 등급 토큰이 존재하면 return
        for (int i = tokenID; i < Weakness3::ID; ++i)
        {
            if (tokenInventory.HasTokenFromID(i))
                return;
        }
        damage = ContentMath::CeilPercentage(damage, 100 - param);
        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
    void Weakness::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                    int& damage)
    {
        const int   tokenID     = GetTokenID();
        const int   param       = GetTokenParam(0);
        auto& tokenInventory = attackerData.Source.GetTokenInventory();
        // 자신보다 높은 등급 토큰이 존재하면 return
        for (int i = tokenID; i < Weakness3::ID; ++i)
        {
            if (tokenInventory.HasTokenFromID(i))
                return;
        }
        damage = ContentMath::CeilPercentage(damage, 100 - param);
        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
}
