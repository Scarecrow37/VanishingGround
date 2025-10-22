#include "pchScripts.h"
#include "SlowToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Slow1)
    REGISTER_TOKEN(Slow2)
    REGISTER_TOKEN(Slow3)
    void Slow::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        int param = GetTokenParam(0);
        speed -= param;
    }
    void Slow::OnRoundStart(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
} // namespace TokenObject