#include "pchScripts.h"
#include "RageToken.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Token/TokenInventory.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Rage1)
    REGISTER_TOKEN(Rage2)

    void Rage::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage)
    {
        const int   tokenID = GetTokenID();
        const int   param   = GetTokenParam(0);
        auto&       tokenInventory = attackerData.Source.GetTokenInventory();

        // 자신보다 높은 등급 토큰이 존재하면 return
        if (tokenID < Rage2::ID)
        {
            for (int i = tokenID + 1; i <= Rage2::ID; ++i)
            {
                if (tokenInventory.HasTokenFromID(i))
                    return;
            }
        }

        damage += ContentMath::CeilPercentage(damage, param);

        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
    void Rage::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)
    {
        const int tokenID = GetTokenID();
        const int param   = GetTokenParam(0);
        auto& tokenInventory = attackerData.Source.GetTokenInventory();

        // 자신보다 높은 등급 토큰이 존재하면 return
        if (tokenID < Rage2::ID)
        {
            for (int i = tokenID + 1; i <= Rage2::ID; ++i)
            {
                if (tokenInventory.HasTokenFromID(i))
                    return;
            }
        }

        damage += ContentMath::CeilPercentage(damage, param);
        tokenInventory.RemoveTokenStackFromID(tokenID);

        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(attackerData.Source));
    }
} // namespace TokenObject