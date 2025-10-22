#include "pchScripts.h"
#include "BleedToken.h"

#include <TurnSystem/TurnActor/Character/CharacterBase.h>
#include <Token/TokenInventory.h>

namespace TokenObject
{
    REGISTER_TOKEN(Bleed)

    REFLECT_FUNCTION(Bleed)

    void Bleed::OnRoundStart(CharacterBase* owner)
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            if (owner->State != TurnActor::STATE::Dead)
            {
                int stackCount = tokenInventory.GetTokenStackFromID(ID);
                UmLogger.Log(LogLevel::LEVEL_DEBUG, TokenLog(*owner));
                int damage = ReflectFields->TickDamage * stackCount;
                owner->TakeDamage(damage);
            }
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
} // namespace TokenObject