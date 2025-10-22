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
    void Obsession::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats,
                                                     WeaponStats& weaponStats, QTE::NoteResult& noteResult,
                                                     Enemy& target, EnemyStats& targetStats)
    {
        // 일격 데미지 감소
        if (QTE::QTE_RESULT_NORMAL == noteResult.Result)
        {
            int   param           = GetTokenParam(0);
            float factor          = static_cast<float>(param) / 100.0f;
            float damage          = static_cast<float>(weaponStats.HitDamage);
            weaponStats.HitDamage = static_cast<int>(damage * (1.0f - factor));
        }
    }
} // namespace TokenObject