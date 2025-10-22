#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REFLECT_FUNCTION(Armor1)
    REGISTER_TOKEN(Armor2)
    REFLECT_FUNCTION(Armor2)
    REGISTER_TOKEN(Armor3)
    REFLECT_FUNCTION(Armor3)
    void Armor1::OnTakeDamage(CharacterBase* source, int& damage)
    {
        float newDamage = static_cast<float>(damage) * (1.0f - ReflectFields->DamageReductionFactor);
        damage = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void Armor2::OnTakeDamage(CharacterBase* source, int& damage) 
    {
        float newDamage = static_cast<float>(damage) * (1.0f - ReflectFields->DamageReductionFactor);
        damage = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
    void Armor3::OnTakeDamage(CharacterBase* source, int& damage) 
    {
        float newDamage = static_cast<float>(damage) * (1.0f - ReflectFields->DamageReductionFactor);
        damage = static_cast<int>(std::ceilf(newDamage));
        if (source)
        {
            TokenInventory& tokenInventory = source->GetTokenInventory();
            tokenInventory.RemoveTokenStackFromID(ID);
        }
    }
} // namespace TokenObject