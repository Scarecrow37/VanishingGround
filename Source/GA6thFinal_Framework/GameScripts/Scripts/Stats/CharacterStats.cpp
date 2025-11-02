#include "pchScripts.h"
#include "CharacterStats.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REFLECT_FUNCTION(CharacterStats)

int CharacterStats::GetMaxChainRoundCout()
{
    int maxChainRoundCount = ReflectFields->MaxChainRoundCount;
    if (TurnMode* mode = SingletonComponent<TurnMode>::GetInstance())
    {
        mode->ApplyActions([&maxChainRoundCount](TurnAction& action) 
        {

        });

    }
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
