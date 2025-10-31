#include "pchScripts.h"
#include "CharacterStats.h"

REFLECT_FUNCTION(CharacterStats)

void CharacterStats::ApplyReduce()
{
    _reduceHpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
}

void CharacterStats::DeserializedReflectEvent() 
{
    _hpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
    _sturnResistanceModel = ReflectFields->StunResistance;
}
