#include "pchScripts.h"
#include "FocusToken.h"
#include "Token/TokenInventory.h"
#include "QTE/Result/QTEResult.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

namespace TokenObject
{
    REGISTER_TOKEN(Focus)
    void Focus::OnPreAttackBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, WeaponStats& weaponStats,
                                          QTE::NoteResult& noteResult, Enemy& target, EnemyStats& targetStats)
    {
        if (QTE::QTE_RESULT_NORMAL == noteResult.Result)
        {
            noteResult.Result = QTE::QTE_RESULT_PERFECT;
        }
        TokenInventory& tokenInventory = attacker.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(ID);
    }
}
