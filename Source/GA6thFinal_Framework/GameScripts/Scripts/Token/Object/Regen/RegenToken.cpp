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
        int param = GetTokenParam(0);
        Heal(owner, param);
        if (owner)
        {
            auto& tokenInventory = owner->GetTokenInventory();
            int   tokenID        = GetTokenID();
            tokenInventory.RemoveTokenStackFromID(tokenID);
        }
    }
} // namespace TokenObject