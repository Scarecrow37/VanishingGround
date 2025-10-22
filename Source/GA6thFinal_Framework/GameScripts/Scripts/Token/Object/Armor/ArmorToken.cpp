#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REGISTER_TOKEN(Armor2)
    REGISTER_TOKEN(Armor3)

    namespace
    {
        float CalculateDamageReductionFactor(int param)
        {
            return static_cast<float>(param) / 100.0f;
        }
    }

    void Armor1::OnTakeDamage(CharacterBase* source, int& damage)
    {
        int     param       = GetTokenParam(0);
        float   factor      = static_cast<float>(param) / 100.0f;
        float   newDamage   = static_cast<float>(damage) * (1.0f - factor);
        damage = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void Armor2::OnTakeDamage(CharacterBase* source, int& damage) 
    {
        int   param     = GetTokenParam(0);
        float factor    = static_cast<float>(param) / 100.0f;
        float newDamage = static_cast<float>(damage) * (1.0f - factor);
        damage          = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void Armor3::OnTakeDamage(CharacterBase* source, int& damage) 
    {
        int   param     = GetTokenParam(0);
        float factor    = static_cast<float>(param) / 100.0f;
        float newDamage = static_cast<float>(damage) * (1.0f - factor);
        damage          = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
} // namespace TokenObject