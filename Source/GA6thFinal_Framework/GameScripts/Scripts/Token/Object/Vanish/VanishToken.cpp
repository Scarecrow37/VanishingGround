#include "pchScripts.h"
#include "VanishToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Weapon/WeaponStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Vanish)

    void Vanish::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                   int& damage)
    {
        TokenInventory& tokenInventory  = attackerData.Source.GetTokenInventory();
        const int   count               = tokenInventory.GetTokenStackFromID(ID);
        const int   param               = GetTokenParam(0) * count;
        const float factor              = 1.0f + (static_cast<float>(param) / 100.0f);
        const float damageFloat         = std::ceilf(static_cast<float>(damage) * factor);

        damage = static_cast<int>(damageFloat);
    }

    void Vanish::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)
    {
        TokenInventory& tokenInventory = attackerData.Source.GetTokenInventory();
        const int       count          = tokenInventory.GetTokenStackFromID(ID);
        const int       param          = GetTokenParam(0) * count;
        const float     factor         = 1.0f + (static_cast<float>(param) / 100.0f);
        const float     damageFloat    = std::ceilf(static_cast<float>(damage) * factor);

        damage = static_cast<int>(damageFloat);
    }
} // namespace TokenObject