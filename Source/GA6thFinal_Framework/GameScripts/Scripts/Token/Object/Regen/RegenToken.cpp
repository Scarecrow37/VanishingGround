#include "pchScripts.h"
#include "RegenToken.h"
#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Regen1)
    REGISTER_TOKEN(Regen2)
    REGISTER_TOKEN(Regen3)
    void Regen::OnTurnStart(CharacterBase* owner) 
    {
        if (owner)
        {
            const int   tokenID = GetTokenID();
            const int   param   = GetTokenParam(0);
            owner->HealByPercentage(param);
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
            UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(*owner));
        }
    }
} // namespace TokenObject