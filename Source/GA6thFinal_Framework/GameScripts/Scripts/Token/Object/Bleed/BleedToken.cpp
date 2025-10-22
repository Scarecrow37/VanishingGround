#include "pchScripts.h"
#include "BleedToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>

namespace TokenObject
{
    REGISTER_TOKEN(Bleed)

    void Bleed::OnRoundStart(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (owner->State != TurnActor::STATE::Dead)
            {
                int stackCount = tokenInventory.GetTokenStackFromID(ID);
                int param  = GetTokenParam(0);
                int damage = param * stackCount;

                UmLogger.Log(LogLevel::LEVEL_DEBUG, TokenLog(*owner));
                owner->TakeDamage(damage);
            }
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
} // namespace TokenObject