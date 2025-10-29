#include "pchScripts.h"
#include "HasteToken.h"
#include "Token/TokenSystem.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Haste1)
    REGISTER_TOKEN(Haste2)
    REGISTER_TOKEN(Haste3)
    void Haste::OnRollRandomSpeed(CharacterBase* source, int& speed) 
    {
        int param = GetTokenParam(0);
        speed += param;
    }
    void Haste::OnRoundStart(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
            UmLogger.Log(LogLevel::LEVEL_TRACE, TokenLog(*owner));
        }
    }
} // namespace TokenObject