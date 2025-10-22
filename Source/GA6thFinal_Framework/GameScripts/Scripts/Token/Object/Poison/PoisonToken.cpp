#include "pchScripts.h"
#include "PoisonToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Poison)
    REFLECT_FUNCTION(Poison)

    void Poison::OnRoundStart(CharacterBase* owner) 
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