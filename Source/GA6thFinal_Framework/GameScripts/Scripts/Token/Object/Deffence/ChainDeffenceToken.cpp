#include "pchScripts.h"
#include "ChainDeffenceToken.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnActor/Character/Enemy/Enemy.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"
#include "Stats/Weapon/WeaponStats.h"
#include "Token/TokenInventory.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(ChainDeffence2)
    REGISTER_TOKEN(ChainDeffence3)
    REGISTER_TOKEN(ChainDeffence4)
    REGISTER_TOKEN(ChainDeffence5)
    REGISTER_TOKEN(ChainDeffence6)
    REGISTER_TOKEN(ChainDeffence7)
    REGISTER_TOKEN(ChainDeffence8)

    void ChainDeffence::OnRoundEnd(CharacterBase* owner)
    {
        if (owner && false == owner->IsDead())
        {
            const int tokenID = GetTokenID();
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
    void ChainDeffence::OnPostPlayerHitCalculateDamage(EnemyAttackData& attackerData, PlayerHitData& targetData,
                                                       int& damage)
    {
        // 대상의 연격 수와 동일할 때 데미지 감소
        const int targetChainCount   = targetData.SourceStats.CurrentChainCount;
        const int deffenceChainCount = GetDeffenceChainCount();
        if (targetChainCount == deffenceChainCount)
        {
            const int param = GetTokenParam(0);
            damage -= ContentMath::CeilPercentage(damage, param);
        }
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }

    void ChainDeffence::OnPostEnemyHitCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                      int& damage)
    {
        // 대상의 연격 수와 동일할 때 데미지 감소
        const int targetChainCount   = targetData.SourceStats.CurrentChainCount;
        const int deffenceChainCount = GetDeffenceChainCount();
        if (targetChainCount == deffenceChainCount)
        {
            const int param = GetTokenParam(0);
            damage -= ContentMath::CeilPercentage(damage, param);
        }
        UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(targetData.Source));
    }

} // namespace TokenObject