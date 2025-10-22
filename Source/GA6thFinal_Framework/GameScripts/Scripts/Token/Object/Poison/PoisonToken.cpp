#include "pchScripts.h"
#include "PoisonToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Poison)
    REGISTER_TOKEN(PoisonGrant)

    void Poison::OnRoundStart(CharacterBase* owner) 
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
    void PoisonGrant::OnTurnEnd(CharacterBase* owner) 
    {
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void PoisonGrant::OnTakeDamage(CharacterBase* source, CharacterBase* dest, int& damage, QTE::NoteResult* noteResult)
    {
        if (damage > 0 && dest)
        {
            auto& tokenInventory = dest->GetTokenInventory();
            const int param      = GetTokenParam(0);
            tokenInventory.AddTokenStackFromID(Poison::ID, param);
        }
    }
} // namespace TokenObject