#include "pchScripts.h"
#include "RegenToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/CharacterStats.h"

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
    }
    void Regen3::OnTurnStart(CharacterBase* owner) 
    {
        Heal(owner, GetTokenParam(0));
    }
    void Regen1::OnTurnStart(CharacterBase* owner) 
    {
        Heal(owner, GetTokenParam(0));
    }
    void Regen2::OnTurnStart(CharacterBase* owner) 
    {
        Heal(owner, GetTokenParam(0));
    }
} // namespace TokenObject