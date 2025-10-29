#include "pchScripts.h"
#include "CharacterStats.h"

REFLECT_FUNCTION(CharacterStats)

void CharacterStats::DeserializedReflectEvent() 
{
    _hpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
    _sturnResistanceModel = ReflectFields->StunResistance;
}
