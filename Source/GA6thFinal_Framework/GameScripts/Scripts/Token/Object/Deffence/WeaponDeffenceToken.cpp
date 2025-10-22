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
    void WeaponDeffence::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                         WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                         Enemy& target, EnemyStats& targetStats)
    {
        // 공격 무기가 무기 방어 타입이랑 같을 때 데미지 감소
        const WeaponType weaponType = weaponStats.Type;
        const WeaponType deffenceType = GetDeffenceWeaponType();
        if (weaponType == deffenceType)
        {
            const int   param  = GetTokenParam(0);
            const float factor = static_cast<float>(param) / 100.0f;

            // TODO: 이거 100%방어가 아예 무시인건지, 아니면 합연산을 통해 무조건 100% 방어가 안될 수도 있는건지 확인 필요
            weaponStats.HitDamageMultiplier -= factor;
        }
    }
}