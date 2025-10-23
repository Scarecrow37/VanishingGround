#include "pchScripts.h"
#include "FocusToken.h"
#include "Token/TokenInventory.h"
#include "QTE/Result/QTEResult.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

namespace TokenObject
{
    REGISTER_TOKEN(Focus)
    void Focus::OnPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)
    {
        if (QTE::QTE_RESULT_NORMAL == attackerData.NoteResult.Result)
        {
            attackerData.NoteResult.Result = QTE::QTE_RESULT_PERFECT;
        }
        TokenInventory& tokenInventory = attackerData.Source.GetTokenInventory();
        tokenInventory.RemoveTokenStackFromID(ID);
    }
}
