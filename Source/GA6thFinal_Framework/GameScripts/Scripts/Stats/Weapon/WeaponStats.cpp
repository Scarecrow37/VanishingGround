#include "pchScripts.h"
#include "WeaponStats.h"
#include "TurnSystem/TurnActor/Character/Player/Player.h"
#include "TurnSystem/TurnMode/TurnMode.h"

REFLECT_FUNCTION(WeaponStats)

int WeaponStats::RollRandomSpeed()
{
    _randomSpeed = Random::Range(TurnActor::DEFINE::RANDOMSPEED_MIN, TurnActor::DEFINE::RANDOMSPEED_MAX);
    if (TurnMode* turnMode = SingletonComponent<TurnMode>::GetInstance())
    {
        if (Player* player = turnMode->GetPlayer())
        {
            TokenInventory& tokenInventory = player->GetTokenInventory();
            tokenInventory.NotifyRollRandomSpeed(_randomSpeed);
        }
    }
    return _randomSpeed;
}