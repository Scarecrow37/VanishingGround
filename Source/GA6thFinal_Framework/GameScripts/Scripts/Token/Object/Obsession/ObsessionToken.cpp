#include "pchScripts.h"
#include "ObsessionToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Weapon/WeaponStats.h"
#include "ContentMath/ContentMath.h"

namespace TokenObject
{
    REGISTER_TOKEN(Obsession1)
    REGISTER_TOKEN(Obsession2)
    REGISTER_TOKEN(Obsession3)

    void Obsession::OnTurnEnd(CharacterBase* owner)
    {
        int tokenID = GetTokenID();
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
    void Obsession::OnPostPlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData,
                                                      int& damage)
    {
        // 일격 데미지 감소
        if (QTE::QTE_RESULT_NORMAL == attackerData.NoteResult.Result)
        {
            const int   param = GetTokenParam(0);
            damage -= ContentMath::CeilPercentage(damage, param);
        }
    }
} // namespace TokenObject