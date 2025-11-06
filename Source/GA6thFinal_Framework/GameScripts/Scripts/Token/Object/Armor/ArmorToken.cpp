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
        damage              = ContentMath::CeilPercentage(damage, 100 -param);
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }
    void Armor::OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage) 
    {
        const int   tokenID = GetTokenID();
        const int   param   = GetTokenParam(0);
        damage              = ContentMath::CeilPercentage(damage, 100 -param);
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }
} // namespace TokenObject