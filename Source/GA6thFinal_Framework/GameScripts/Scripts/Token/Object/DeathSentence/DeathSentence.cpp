#include "pchScripts.h"
#include "DeathSentence.h"
#include "Token/TokenInventory.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(DeathSentence)

    void DeathSentence::OnRoundStart(CharacterBase* owner)
    {
        int tokenID = GetTokenID();
        if (owner)
        {
            TokenInventory& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);

            // 스택이 0이 되면 즉시 사망
            int count = tokenInventory.GetTokenStackFromID(tokenID);
            if (0 >= count)
            {
                owner->Dead();
            }
        }
    }
}