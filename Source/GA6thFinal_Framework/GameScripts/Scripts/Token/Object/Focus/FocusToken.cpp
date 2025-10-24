#include "pchScripts.h"
#include "FocusToken.h"
#include "Token/TokenInventory.h"
#include "QTE/Result/QTEResult.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"

namespace TokenObject
{
    REGISTER_TOKEN(Focus)

    void Focus::OnTurnEnd(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void Focus::OnPrePlayerAttackCalculateDamage(PlayerAttackData& attackerData, EnemyHitData& targetData)
    {
        // TODO: QTE할때 직접 확인할까? 이펙트때문에라도 여기다 하면 안될 것 같음.
        if (QTE::QTE_RESULT_NORMAL == attackerData.NoteResult.Result)
        {
            attackerData.NoteResult.Result = QTE::QTE_RESULT_PERFECT;
        }
    }
}
