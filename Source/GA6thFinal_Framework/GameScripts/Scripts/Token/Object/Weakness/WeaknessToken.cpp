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
        damage = ContentMath::CeilPercentage(damage, 100 - param);

        auto& tokenInventory = attackerData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
    void Weakness::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                    int& damage)
    {
        const int   tokenID     = GetTokenID();
        const int   param       = GetTokenParam(0);
        damage = ContentMath::CeilPercentage(damage, 100 - param);

        auto& tokenInventory = attackerData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
}
