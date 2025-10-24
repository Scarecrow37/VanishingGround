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
}
