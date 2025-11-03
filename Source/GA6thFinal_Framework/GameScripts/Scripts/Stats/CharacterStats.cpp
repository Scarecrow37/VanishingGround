#include "pchScripts.h"
#include "CharacterStats.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REFLECT_FUNCTION(CharacterStats)

int CharacterStats::GetMaxChainRoundCout()
{
    int maxChainRoundCount = ReflectFields->MaxChainRoundCount;
    return maxChainRoundCount;
}

void CharacterStats::ApplyReduce()
{
    _reduceHpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
}

void CharacterStats::DeserializedReflectEvent() 
{
    _hpModel = std::make_pair(_currentHP, ReflectFields->MaxHP);
    _sturnResistanceModel = ReflectFields->StunResistance;
}
