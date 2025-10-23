#include "pchScripts.h"
#include "WeaponDeffenceToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Token/TokenInventory.h"

namespace TokenObject
{
    REGISTER_TOKEN(SwordDeffence1)
    REGISTER_TOKEN(SwordDeffence2)
    REGISTER_TOKEN(DaggerDeffence1)
    REGISTER_TOKEN(DaggerDeffence2)
    REGISTER_TOKEN(HammerDeffence1)
    REGISTER_TOKEN(HammerDeffence2)

    void WeaponDeffence::OnRoundStart(CharacterBase* owner)
    {
        if (owner && false == owner->IsDead())
        {
            const int tokenID        = GetTokenID();
            auto&     tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
    void WeaponDeffence::OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                       int& damage)
    {
        // 공격 무기가 무기 방어 타입이랑 같을 때 데미지 감소
        const WeaponType weaponType   = attackerData.WeaponStats.Type;
        const WeaponType deffenceType = GetDeffenceWeaponType();
        if (weaponType == deffenceType)
        {
            const int   param     = GetTokenParam(0);
            const float factor    = 1.0f - (static_cast<float>(param) / 100.0f);
            const float newDamage = static_cast<float>(damage) * factor;
            damage                = static_cast<int>(std::ceilf(newDamage));
        }
    }
}