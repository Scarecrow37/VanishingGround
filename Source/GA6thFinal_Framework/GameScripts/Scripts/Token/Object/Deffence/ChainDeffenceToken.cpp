#include "pchScripts.h"
#include "ChainDeffenceToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Token/TokenInventory.h"

namespace TokenObject
{
    REGISTER_TOKEN(ChainDeffence2)
    REGISTER_TOKEN(ChainDeffence3)
    REGISTER_TOKEN(ChainDeffence4)
    REGISTER_TOKEN(ChainDeffence5)
    REGISTER_TOKEN(ChainDeffence6)
    REGISTER_TOKEN(ChainDeffence7)
    REGISTER_TOKEN(ChainDeffence8)

    void ChainDeffence::OnRoundStart(CharacterBase* owner)
    {
        if (owner && false == owner->IsDead())
        {
            const int tokenID = GetTokenID();
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
    void ChainDeffence::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                  WeaponStats& weaponStats, QTE::NoteResult& noteResult, Enemy& target,
                                                  EnemyStats& targetStats)
    {
        // 대상의 연격 수와 동일할 때 데미지 감소
        const int targetChainCount = targetStats.CurrentChainCount;
        const int deffenceChainCount = GetDeffenceChainCount();
        if (targetChainCount == deffenceChainCount)
        {
            const int param = GetTokenParam(0);
            const float factor = static_cast<float>(param) / 100.0f;

            // TODO: 이거 100%방어가 아예 무시인건지, 아니면 합연산을 통해 무조건 100% 방어가 안될 수도 있는건지 확인 필요
            weaponStats.HitDamageMultiplier -= factor;
            weaponStats.CriticalDamageMultiplier -= factor;
        }
    }

    void ChainDeffence::OnPreAttackBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                                         PlayerStats& targetStats)
    {
        // 대상의 연격 수와 동일할 때 데미지 감소
        const int targetChainCount = targetStats.CurrentChainCount;
        const int deffenceChainCount = GetDeffenceChainCount();
        if (targetChainCount == deffenceChainCount)
        {
            const int   param  = GetTokenParam(0);
            const float factor = static_cast<float>(param) / 100.0f;

            attackerStats.DamageMultiplier -= factor;
        }
    }

} // namespace TokenObject