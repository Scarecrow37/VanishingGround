#include "pchScripts.h"
#include "ObsessionToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "Stats/Weapon/WeaponStats.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Obsession1)
    REGISTER_TOKEN(Obsession2)
    REGISTER_TOKEN(Obsession3)

    void Obsession::OnTurnEnd(CharacterBase* owner)
    {
        const int tokenID = GetTokenID();
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            // 이번 턴을 제외한 사용 가능 토큰이 있는 경우
            if (tokenInventory.IsAvailableTokenFromID(tokenID))
            {
                tokenInventory.RemoveTokenStackFromID(tokenID);
            }
        }
    }
    void Obsession::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                      int& damage)
    {
        auto& tokenInventory = attackerData.Source.GetTokenInventory();
        // 이번 턴을 제외한 사용 가능 토큰이 있는 경우
        if (tokenInventory.IsAvailableTokenFromID(GetTokenID()))
        {
            // 일격 데미지 감소
            if (QTE::QTE_RESULT_NORMAL == attackerData.NoteResult.Result)
            {
                const int param = GetTokenParam(0);
                damage          = ContentMath::CeilPercentage(damage, 100 - param);
            }
        }
    }
} // namespace TokenObject