#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REGISTER_TOKEN(Armor2)
    REGISTER_TOKEN(Armor3)

    void Armor::OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage) 
    {
        const int   tokenID            = GetTokenID();
        const int   param              = GetTokenParam(0);
        const float factor             = 1.0f - (static_cast<float>(param) / 100.0f);
        const float newDamage          = static_cast<float>(damage) * factor;
        damage                         = static_cast<int>(std::ceilf(newDamage));
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
    void Armor::OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData, int& damage) 
    {
        const int   tokenID            = GetTokenID();
        const int   param              = GetTokenParam(0);
        const float factor             = 1.0f - (static_cast<float>(param) / 100.0f);
        const float newDamage          = static_cast<float>(damage) * factor;
        damage                         = static_cast<int>(std::ceilf(newDamage));
        TokenInventory& tokenInventory = targetData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(tokenID);
    }
} // namespace TokenObject