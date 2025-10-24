#include "pchScripts.h"
#include "ObsessionToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Weapon/WeaponStats.h"

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
            const int   param     = GetTokenParam(0);
            const float factor    = 1.0f - (static_cast<float>(param) / 100.0f);
            const float newDamage = static_cast<float>(damage) * factor;
            damage                = static_cast<int>(std::ceilf(newDamage));
        }
    }
} // namespace TokenObject