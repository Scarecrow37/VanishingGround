#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REGISTER_TOKEN(Armor2)
    REGISTER_TOKEN(Armor3)

    void Armor::OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage) 
    {
       
        const int   tokenID = GetTokenID();
        const int   param   = GetTokenParam(0);
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();

        // 자신보다 높은 등급 토큰이 존재하면 return
        if (tokenID < Armor3::ID)
        {
            for (int i = tokenID + 1; i <= Armor3::ID; ++i)
            {
                if (tokenInventory.HasTokenFromID(i))
                    return;
            }
        }

        damage  = ContentMath::CeilPercentage(damage, 100 -param);
        tokenInventory.RemoveTokenStackFromID(tokenID);
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }
    void Armor::OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage) 
    {
        const int   tokenID = GetTokenID();
        const int   param   = GetTokenParam(0);
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();

        // 자신보다 높은 등급 토큰이 존재하면 return
        if (tokenID < Armor3::ID)
        {
            for (int i = tokenID + 1; i <= Armor3::ID; ++i)
            {
                if (tokenInventory.HasTokenFromID(i))
                    return;
            }
        }

        damage = ContentMath::CeilPercentage(damage, 100 -param);
        tokenInventory.RemoveTokenStackFromID(tokenID);
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }
} // namespace TokenObject