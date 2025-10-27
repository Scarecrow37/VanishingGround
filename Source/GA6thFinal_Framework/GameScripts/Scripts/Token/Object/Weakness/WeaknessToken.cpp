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
    void Weakness::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                     int& damage)
    {
        const int   tokenID     = GetTokenID();
        const int   param       = GetTokenParam(0);
        const float factor      = 1.0f -(static_cast<float>(param) / 100.0f);
        const float damageFloat = std::ceilf(static_cast<float>(damage) * factor);

        damage = static_cast<int>(damageFloat);
    }
    void Weakness::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                    int& damage)
    {
        const int   tokenID     = GetTokenID();
        const int   param       = GetTokenParam(0);
        const float factor      = 1.0f - (static_cast<float>(param) / 100.0f);
        const float damageFloat = std::ceilf(static_cast<float>(damage) * factor);

        damage = static_cast<int>(damageFloat);
    }
}
