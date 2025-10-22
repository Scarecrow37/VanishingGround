#include "pchScripts.h"
#include "ArmorToken.h"
#include "TurnSystem/TurnActor/Character/CharacterBase.h"
#include "Stats/Enemy/EnemyStats.h"
#include "Stats/Player/PlayerStats.h"

namespace TokenObject
{
    REGISTER_TOKEN(Armor1)
    REFLECT_FUNCTION(Armor1)
    void Armor1::OnPreHitBattleCalculateDamage(Player& attacker, PlayerStats& attackerStats, Enemy& target,
                                               EnemyStats& targetStats)
    {
    }
    void Armor1::OnPreHitBattleCalculateDamage(Enemy& attacker, EnemyStats& attackerStats, Player& target,
                                               PlayerStats& targetStats)
    {
    }
} // namespace TokenObject