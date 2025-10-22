#include "pchScripts.h"
#include "WeaponStats.h"
#include <TurnSystem/TurnActor/TurnActor.h>

REFLECT_FUNCTION(WeaponStats)

int WeaponStats::RollRandomSpeed()
{
    return _randomSpeed = Random::Range(TurnActor::DEFINE::RANDOMSPEED_MIN, TurnActor::DEFINE::RANDOMSPEED_MAX);
}