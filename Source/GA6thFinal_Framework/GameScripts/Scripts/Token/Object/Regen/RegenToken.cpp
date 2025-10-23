#include "pchScripts.h"
#include "RegenToken.h"
#include "Stats/CharacterStats.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"

namespace TokenObject
{
    REGISTER_TOKEN(Regen1)
    REGISTER_TOKEN(Regen2)
    REGISTER_TOKEN(Regen3)
    namespace
    {
        void Heal(CharacterBase* dest, int healFactor)
        {
            if (CharacterStats* stats = dest->GetCharacterStats())
            {
                int   maxHP      = dest->MaxHP;
                float factor     = static_cast<float>(healFactor) / 100.0f;
                float healAmount = static_cast<float>(maxHP) * factor;
                stats->CurrentHP += static_cast<int>(healAmount);
            }
        }
    } // namespace
    void Regen::OnTurnStart(CharacterBase* owner) 
    {
        if (owner)
        {
            const int   tokenID = GetTokenID();
            const int   param   = GetTokenParam(0);
            const float factor  = static_cast<float>(param) / 100.0f;
            owner->Heal(factor);
            auto& tokenInventory = owner->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
} // namespace TokenObject