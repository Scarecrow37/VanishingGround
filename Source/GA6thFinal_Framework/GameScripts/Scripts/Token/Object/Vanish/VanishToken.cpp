#include "pchScripts.h"
#include "VanishToken.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Weapon/WeaponStats.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Vanish)

    void Vanish::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                   int& damage)
    {
        TokenInventory& tokenInventory  = attackerData.Source.GetTokenInventory();
        const int   count               = tokenInventory.GetTokenStackFromID(ID);
        const int   param               = GetTokenParam(0) * count;
        damage += ContentMath::CeilPercentage(damage, param);
    }

    void Vanish::OnPostEnemyAttackCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData, int& damage)
    {
        TokenInventory& tokenInventory = attackerData.Source.GetTokenInventory();
        const int       count          = tokenInventory.GetTokenStackFromID(ID);
        const int       param          = GetTokenParam(0) * count;
        damage += ContentMath::CeilPercentage(damage, param);
    }
} // namespace TokenObject