#include "pchScripts.h"
#include "CharacterStats.h"

void CharacterStats::DeserializedReflectEvent() 
{
    _hpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
    _sturnResistanceModel = ReflectFields->StunResistance;
}
